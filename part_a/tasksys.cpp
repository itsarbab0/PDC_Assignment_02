#include "tasksys.h"
////  Adding respective Lib
#include<iostream>
#include<string>
#include<vector>
#include<thread>
#include<mutex>
#include<queue>
#include<condition_variable>
using namespace std;

IRunnable::~IRunnable() {}

ITaskSystem::ITaskSystem(int num_threads) {}
ITaskSystem::~ITaskSystem() {}

/*
 * ================================================================
 * Serial task system implementation
 * ================================================================
 */

const char* TaskSystemSerial::name() {
    return "Serial";
}

TaskSystemSerial::TaskSystemSerial(int num_threads): ITaskSystem(num_threads) {

}

TaskSystemSerial::~TaskSystemSerial() {}

void TaskSystemSerial::run(IRunnable* runnable, int num_total_tasks) {
    for (int i = 0; i < num_total_tasks; i++) {
        runnable->runTask(i, num_total_tasks);
    }
}

TaskID TaskSystemSerial::runAsyncWithDeps(IRunnable* runnable, int num_total_tasks,
                                          const vector<TaskID>& deps) {
    // You do not need to implement this method.
    return 0;
}

void TaskSystemSerial::sync() {
    // You do not need to implement this method.
    return;
}

/*
 * ================================================================
 * Parallel Task System Implementation
 * ================================================================
 */

const char* TaskSystemParallelSpawn::name() {
    return "Parallel + Always Spawn";
}

TaskSystemParallelSpawn::TaskSystemParallelSpawn(int num_threads): ITaskSystem(num_threads) {
    //
    // TODO: CS149 student implementations may decide to perform setup
    // operations (such as thread pool construction) here.
    // Implementations are free to add new class member variables
    // (requiring changes to tasksys.h).
    //
}

TaskSystemParallelSpawn::~TaskSystemParallelSpawn() {}

void TaskSystemParallelSpawn::run(IRunnable* runnable, int num_total_tasks) {


    //
    // TODO: CS149 students will modify the implementation of this
    // method in Part A.  The implementation provided below runs all
    // tasks sequentially on the calling thread.
    //

    // for (int i = 0; i < num_total_tasks; i++) {
    //     runnable->runTask(i, num_total_tasks);
    // }

    ///// Now I am implementing my Logic for  Task Parallel with Spawn
    vector<thread> threadsarr;     // Making array for threads

    // Now Spawn new thread for each task
    for(int i = 0; i < num_total_tasks; i++)
    {
        threadsarr.push_back(thread([i, runnable, num_total_tasks](){
            runnable->runTask(i, num_total_tasks);
        }));
    }

    /// Waiting for each thred
    for(auto& t : threadsarr)
    {
        t.join();
    }


    ////   Second approach

    // vector<thread> arr;
    // queue<int> tq;
    // mutex mtxlock;
    // condition_variable vari;
    // bool taskDone = false;
    // int num_threads = 16;
    // /// Filling queue with tasks
    // for(int i = 0; i < num_total_tasks; i++)
    // {
    //     tq.push(i);
    // }

    // for(int i = 0; i < num_threads; i++)
    // {
    //     arr.push_back(thread([&tq, &mtxlock, &vari, &taskDone, runnable, num_total_tasks]() 
    //     {
    //         while (true) 
    //         {
    //             unique_lock<mutex> lock(mtxlock);
                
                
    //             vari.wait(lock, [&tq, &taskDone]() { return !tq.empty() || taskDone; });

    //             if (taskDone && tq.empty())
    //             { 
    //                 break;
    //             }

    //             int taskid = tq.front();
    //             tq.pop();
    //             lock.unlock();

                
    //             runnable->runTask(taskid, num_total_tasks);
    //         }
    //     }));
    // }
    // {
    //     lock_guard<mutex>lock(mtxlock);
    //     vari.notify_all();
    // }
    // for(auto& t : arr)
    // {
    //     t.join();
    // }
    // {
    //     lock_guard<mutex> lock(mtxlock);
    //     taskDone = true;
    // }
    // vari.notify_all();

}

TaskID TaskSystemParallelSpawn::runAsyncWithDeps(IRunnable* runnable, int num_total_tasks,
                                                 const vector<TaskID>& deps) {
    // You do not need to implement this method.
    return 0;
}

void TaskSystemParallelSpawn::sync() {
    // You do not need to implement this method.
    return;
}

/*
 * ================================================================
 * Parallel Thread Pool Spinning Task System Implementation
 * ================================================================
 */

const char* TaskSystemParallelThreadPoolSpinning::name() {
    return "Parallel + Thread Pool + Spin";
}

TaskSystemParallelThreadPoolSpinning::TaskSystemParallelThreadPoolSpinning(int num_threads): ITaskSystem(num_threads) {
    //
    // TODO: CS149 student implementations may decide to perform setup
    // operations (such as thread pool construction) here.
    // Implementations are free to add new class member variables
    // (requiring changes to tasksys.h).
    //
}

TaskSystemParallelThreadPoolSpinning::~TaskSystemParallelThreadPoolSpinning() {}

void TaskSystemParallelThreadPoolSpinning::run(IRunnable* runnable, int num_total_tasks) {


    //
    // TODO: CS149 students will modify the implementation of this
    // method in Part A.  The implementation provided below runs all
    // tasks sequentially on the calling thread.
    //

    // for (int i = 0; i < num_total_tasks; i++) {
    //     runnable->runTask(i, num_total_tasks);
    // }

    ///// Now here is my Logoc for TaskSysParalell Thread Pool Spi
    // int num_threads = 16;
    // vector<thread> threadsarr;
    // mutex mtxlock;
    // bool taskDone = false;
    // for(int i = 0; i < num_threads; i++)
    // {
    //     threadsarr.push_back(thread([&mtxlock, &taskDone, this, runnable, num_total_tasks](){
    //         while(true)
    //         {
    //             lock_guard<mutex> lock(mtxlock);
    //             if(taskDone)
    //             {
    //                 break;
    //             }

    //             for(int taskid = 0; taskid < num_total_tasks; taskid++)
    //             {
    //                 runnable->runTask(taskid, num_total_tasks);
    //             }
    //         }
    //     }));
    // }

    // {
    //     lock_guard<mutex> lock(mtxlock);
    //     taskDone = true;
    // }

    // for(auto& t : threadsarr)
    // {
    //     t.join();
    // }



    ////  Second Approach
    vector<thread> arr;
    queue<int> tq;
    mutex mtxlock; 
    condition_variable vari;
    bool taskDone = false;
    int num_threads = 16;
    for (int i = 0; i < num_total_tasks; i++)
    {
        tq.push(i);
    }

    for(int i = 0; i < num_threads; i++)
    {
        arr.push_back(thread([&tq, &mtxlock, &vari, &taskDone, runnable, num_total_tasks]()
        {
            
            while(true)
            {
                unique_lock<mutex> lock(mtxlock);
                vari.wait(lock, [&tq, &taskDone]() { return !tq.empty() || taskDone;});

                if(taskDone && tq.empty())
                {
                    break;
                }

                int tasid = tq.front();
                tq.pop();
                lock.unlock();
                runnable->runTask(tasid, num_total_tasks);
            }
        }));
    }

    {
        lock_guard<mutex> lock(mtxlock);
        vari.notify_all();
    }
    for(auto& t : arr)
    {
        t.join();
    }
    {
        lock_guard<mutex> lock(mtxlock);
        taskDone = true;
    }
    vari.notify_all();

}

TaskID TaskSystemParallelThreadPoolSpinning::runAsyncWithDeps(IRunnable* runnable, int num_total_tasks,
                                                              const vector<TaskID>& deps) {
    // You do not need to implement this method.
    return 0;
}

void TaskSystemParallelThreadPoolSpinning::sync() {
    // You do not need to implement this method.
    return;
}

/*
 * ================================================================
 * Parallel Thread Pool Sleeping Task System Implementation
 * ================================================================
 */

const char* TaskSystemParallelThreadPoolSleeping::name() {
    return "Parallel + Thread Pool + Sleep";
}

TaskSystemParallelThreadPoolSleeping::TaskSystemParallelThreadPoolSleeping(int num_threads): ITaskSystem(num_threads) {
    //
    // TODO: CS149 student implementations may decide to perform setup
    // operations (such as thread pool construction) here.
    // Implementations are free to add new class member variables
    // (requiring changes to tasksys.h).
    //
}

TaskSystemParallelThreadPoolSleeping::~TaskSystemParallelThreadPoolSleeping() {
    //
    // TODO: CS149 student implementations may decide to perform cleanup
    // operations (such as thread pool shutdown construction) here.
    // Implementations are free to add new class member variables
    // (requiring changes to tasksys.h).
    //
}

void TaskSystemParallelThreadPoolSleeping::run(IRunnable* runnable, int num_total_tasks) {


    //
    // TODO: CS149 students will modify the implementation of this
    // method in Parts A and B.  The implementation provided below runs all
    // tasks sequentially on the calling thread.
    //

    // for (int i = 0; i < num_total_tasks; i++) {
    //     runnable->runTask(i, num_total_tasks);
    // }

    // //// Now here I am Implenting  my Logic for TaskSysParalell Thread Pool Sleep
    // int num_threads = 16;
    // vector<thread> threadsarr;
    // mutex mtxlock;
    // condition_variable vari;
    // bool taskDone = false;
    // for(int i = 0; i < num_threads; i++)
    // {
    //     threadsarr.push_back(thread([&vari, &mtxlock, &taskDone, this, runnable, num_total_tasks]()
    //     {
    //         unique_lock<mutex> lock(mtxlock);
    //         while(!taskDone)
    //         {
    //             vari.wait(lock);
    //             for(int taskid = 0; taskid < num_total_tasks; taskid++)
    //             {
    //                 runnable->runTask(taskid, num_total_tasks);
    //             }
    //         }
    //     }));
    // }

    // {
    //     lock_guard<mutex> lock(mtxlock);
    //     taskDone = true;
    // }
    // vari.notify_all();

    // for(auto& t : threadsarr)
    // {
    //     t.join();
    // }



    vector<thread> arr;
    mutex mtxlock;
    condition_variable vari;
    bool taskDone = false;
    int num_threads = 16;

    
    for (int i = 0; i < num_threads; i++) 
    {
        arr.push_back(thread([&mtxlock, &vari, &taskDone, runnable, num_total_tasks]() 
        {
            unique_lock<mutex> lock(mtxlock);

            
            while (!taskDone) 
            {
                vari.wait(lock);  
                for (int taskid = 0; taskid < num_total_tasks; taskid++)
                {
                    runnable->runTask(taskid, num_total_tasks);
                }
            }
        }));
    }


    {
        lock_guard<mutex> lock(mtxlock);
        taskDone = true;  
    }

    vari.notify_all();  

    for (auto& t : arr) 
    {
        t.join();
    }


}

TaskID TaskSystemParallelThreadPoolSleeping::runAsyncWithDeps(IRunnable* runnable, int num_total_tasks,
                                                    const vector<TaskID>& deps) {


    //
    // TODO: CS149 students will implement this method in Part B.
    //

    return 0;
}

void TaskSystemParallelThreadPoolSleeping::sync() {

    //
    // TODO: CS149 students will modify the implementation of this method in Part B.
    //

    return;
}
