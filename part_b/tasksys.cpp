#include "tasksys.h"
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <queue>
#include <unordered_map>
using namespace std;

IRunnable::~IRunnable() {}

ITaskSystem::ITaskSystem(int num_threads) {}
ITaskSystem::~ITaskSystem() {}

/*
 * ================================================================
 * Serial task system implementation
 * ================================================================
 */

const char *TaskSystemSerial::name()
{
    return "Serial";
}

TaskSystemSerial::TaskSystemSerial(int num_threads) : ITaskSystem(num_threads)
{
}

TaskSystemSerial::~TaskSystemSerial() {}

void TaskSystemSerial::run(IRunnable *runnable, int num_total_tasks)
{
    for (int i = 0; i < num_total_tasks; i++)
    {
        runnable->runTask(i, num_total_tasks);
    }
}

TaskID TaskSystemSerial::runAsyncWithDeps(IRunnable *runnable, int num_total_tasks,
                                          const std::vector<TaskID> &deps)
{
    for (int i = 0; i < num_total_tasks; i++)
    {
        runnable->runTask(i, num_total_tasks);
    }

    return 0;
}

void TaskSystemSerial::sync()
{
    return;
}

/*
 * ================================================================
 * Parallel Task System Implementation
 * ================================================================
 */

const char *TaskSystemParallelSpawn::name()
{
    return "Parallel + Always Spawn";
}

TaskSystemParallelSpawn::TaskSystemParallelSpawn(int num_threads) : ITaskSystem(num_threads)
{
    // NOTE: CS149 students are not expected to implement TaskSystemParallelSpawn in Part B.
}

TaskSystemParallelSpawn::~TaskSystemParallelSpawn() {}

void TaskSystemParallelSpawn::run(IRunnable *runnable, int num_total_tasks)
{
    // NOTE: CS149 students are not expected to implement TaskSystemParallelSpawn in Part B.
    for (int i = 0; i < num_total_tasks; i++)
    {
        runnable->runTask(i, num_total_tasks);
    }
}

TaskID TaskSystemParallelSpawn::runAsyncWithDeps(IRunnable *runnable, int num_total_tasks,
                                                 const std::vector<TaskID> &deps)
{
    // NOTE: CS149 students are not expected to implement TaskSystemParallelSpawn in Part B.
    for (int i = 0; i < num_total_tasks; i++)
    {
        runnable->runTask(i, num_total_tasks);
    }

    return 0;
}

void TaskSystemParallelSpawn::sync()
{
    // NOTE: CS149 students are not expected to implement TaskSystemParallelSpawn in Part B.
    return;
}

/*
 * ================================================================
 * Parallel Thread Pool Spinning Task System Implementation
 * ================================================================
 */

const char *TaskSystemParallelThreadPoolSpinning::name()
{
    return "Parallel + Thread Pool + Spin";
}

TaskSystemParallelThreadPoolSpinning::TaskSystemParallelThreadPoolSpinning(int num_threads) : ITaskSystem(num_threads)
{
    // NOTE: CS149 students are not expected to implement TaskSystemParallelThreadPoolSpinning in Part B.
}

TaskSystemParallelThreadPoolSpinning::~TaskSystemParallelThreadPoolSpinning() {}

// void TaskSystemParallelThreadPoolSpinning::run(IRunnable *runnable, int num_total_tasks)
// {
//     // NOTE: CS149 students are not expected to implement TaskSystemParallelThreadPoolSpinning in Part B.
//     for (int i = 0; i < num_total_tasks; i++)
//     {
//         runnable->runTask(i, num_total_tasks);
//     }
// }

// TaskID TaskSystemParallelThreadPoolSpinning::runAsyncWithDeps(IRunnable *runnable, int num_total_tasks,
//                                                               const std::vector<TaskID> &deps)
// {
//     // NOTE: CS149 students are not expected to implement TaskSystemParallelThreadPoolSpinning in Part B.
//     for (int i = 0; i < num_total_tasks; i++)
//     {
//         runnable->runTask(i, num_total_tasks);
//     }

//     return 0;
// }

// void TaskSystemParallelThreadPoolSpinning::sync()
// {
//     // NOTE: CS149 students are not expected to implement TaskSystemParallelThreadPoolSpinning in Part B.
//     return;
// }
void TaskSystemParallelThreadPoolSpinning::run(IRunnable *runnable, int num_total_tasks)
{
    vector<thread> threadsarr;
    mutex taskMutex;
    bool taskDone = false;

    for (int i = 0; i < num_threads; i++)
    {
        threadsarr.push_back(thread([&taskMutex, &taskDone, this, runnable, num_total_tasks]()
                                    {
            std::lock_guard<std::mutex> lock(taskMutex);
            
            if (!taskDone) {
                for (int taskid = 0; taskid < num_total_tasks; taskid++) {
                    runnable->runTask(taskid, num_total_tasks);
                }
            }
        }));
    }

    {
        std::lock_guard<std::mutex> lock(taskMutex);
        taskDone = true;
    }

    for (auto& t : threadsarr)
    {
        t.join();
    }
}

TaskID TaskSystemParallelThreadPoolSpinning::runAsyncWithDeps(IRunnable *runnable, int num_total_tasks,
                                                              const vector<TaskID> &deps)
{
    static TaskID nextTaskID = 0;
    TaskID currentTaskID = nextTaskID++;

    unordered_map<TaskID, int> dependencyCount;
    unordered_map<TaskID, vector<TaskID>> taskDeps;

    for (TaskID dep : deps)
    {
        taskDeps[dep].push_back(currentTaskID);
        dependencyCount[currentTaskID]++;
    }

    vector<thread> threadsarr;
    mutex taskMutex;
    condition_variable cv;
    int num_threads = 16;
    for (int i = 0; i < num_threads; i++)
    {
        threadsarr.push_back(thread([&taskMutex, &cv, this, runnable, num_total_tasks, &dependencyCount, &taskDeps, currentTaskID]()
                                    {
            unique_lock<mutex> lock(taskMutex);
            cv.wait(lock, [&dependencyCount]() { return dependencyCount.empty(); });

            for (int taskid = 0; taskid < num_total_tasks; taskid++) {
                runnable->runTask(taskid, num_total_tasks);
            }

            if (taskDeps.find(currentTaskID) != taskDeps.end()) {
                for (auto& dependentTask : taskDeps[currentTaskID]) {
                    if (--dependencyCount[dependentTask] == 0) {
                        cv.notify_all();
                    }
                }
            }
        }));
    }

    {
        lock_guard<mutex> lock(taskMutex);
        cv.notify_all();
    }

    for (auto& t : threadsarr)
    {
        t.join();
    }

    return currentTaskID;
}

void TaskSystemParallelThreadPoolSpinning::sync()
{
    unique_lock<mutex> lock(taskMutex);
    cv.wait(lock, [this]() { return tasksDone; });
}


/*
 * ================================================================
 * Parallel Thread Pool Sleeping Task System Implementation
 * ================================================================
 */

const char *TaskSystemParallelThreadPoolSleeping::name()
{
    return "Parallel + Thread Pool + Sleep";
}

TaskSystemParallelThreadPoolSleeping::TaskSystemParallelThreadPoolSleeping(int num_threads) : ITaskSystem(num_threads), tasksDone(false), num_threads(num_threads)
{
    //
    // TODO: CS149 student implementations may decide to perform setup
    // operations (such as thread pool construction) here.
    // Implementations are free to add new class member variables
    // (requiring changes to tasksys.h).
    //
}

TaskSystemParallelThreadPoolSleeping::~TaskSystemParallelThreadPoolSleeping()
{
    //
    // TODO: CS149 student implementations may decide to perform cleanup
    // operations (such as thread pool shutdown construction) here.
    // Implementations are free to add new class member variables
    // (requiring changes to tasksys.h).
    //
}

void TaskSystemParallelThreadPoolSleeping::run(IRunnable* runnable, int num_total_tasks) 
{
    vector<thread> threadsarr;
    mutex taskMutex;
    condition_variable cv;

    for (int i = 0; i < num_threads; i++) 
    {
        threadsarr.push_back(thread([&taskMutex, &cv, this, runnable, num_total_tasks]() 
        { 
            unique_lock<mutex> lock(taskMutex);

            
            cv.wait(lock, [this]() { return tasksDone; });  

            
            for (int taskid = 0; taskid < num_total_tasks; taskid++) 
            {
                runnable->runTask(taskid, num_total_tasks);
            }
        }));
    }

  
    {
        lock_guard<mutex> lock(taskMutex);
        tasksDone = true; 
    }

    cv.notify_all();  

    for (auto& t : threadsarr) 
    {
        t.join();
    }
}

TaskID TaskSystemParallelThreadPoolSleeping::runAsyncWithDeps(IRunnable* runnable, int num_total_tasks, const vector<TaskID>& deps) 
{

    static TaskID nextTaskID = 0;
    TaskID currentTaskID = nextTaskID++;  

    unordered_map<TaskID, int> dependencyCount;
    unordered_map<TaskID, vector<TaskID>> taskDeps;

    for (TaskID dep : deps) {
        taskDeps[dep].push_back(currentTaskID);
        dependencyCount[currentTaskID]++;
    }

    vector<thread> threadsarr;
    mutex taskMutex;
    condition_variable cv;

    for (int i = 0; i < num_threads; i++) {
        threadsarr.push_back(thread([&taskMutex, &cv, this, runnable, num_total_tasks, &dependencyCount, &taskDeps, currentTaskID]() 
        {  
            unique_lock<mutex> lock(taskMutex);

            cv.wait(lock, [&dependencyCount]() { return dependencyCount.empty(); });

            
            for (int taskid = 0; taskid < num_total_tasks; taskid++) 
            {
                runnable->runTask(taskid, num_total_tasks);
            }

            if (taskDeps.find(currentTaskID) != taskDeps.end()) 
            {
                for (auto& dependentTask : taskDeps[currentTaskID]) 
                {
                    if (--dependencyCount[dependentTask] == 0) 
                    {
                        cv.notify_all();  
                    }
                }
            }
        }));
    }

    {
        lock_guard<mutex> lock(taskMutex);
        cv.notify_all();
    }

    for (auto& t : threadsarr) {
        t.join();
    }

    return currentTaskID;
}

void TaskSystemParallelThreadPoolSleeping::sync() 
{
    unique_lock<mutex> lock(taskMutex);
    cv.wait(lock, [this]() { return tasksDone; });  
}
