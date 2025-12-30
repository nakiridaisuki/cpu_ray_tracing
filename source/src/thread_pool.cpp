#include "thread_pool.hpp"

void ThreadPool::WorkerThread(ThreadPool *master) {
    // Using avail to check is this thread pool is destructed
    // if not, then get task and run it.

    while(master->avail) {
        Task *task = master->getTask();
        if(task == nullptr){
            // return the resource to OS
            std::this_thread::yield();
            continue;
        }
        task->run();
    }
}

ThreadPool::ThreadPool(size_t thread_count) {
    // Construct the thread pool and create `thread_count` threads
    // if `thread_count` is 0, then create supported number of threads

    avail = true;
    if(thread_count == 0){
        thread_count = std::thread::hardware_concurrency();
    }
    for(size_t i=0; i<thread_count; i++)
        threads.push_back(std::thread(ThreadPool::WorkerThread, this));
}

ThreadPool::~ThreadPool() {
    // Deconstruct the thread pool
    // 1. Waiting until all tasks are taken
    // 2. Set avail to tell all threads to stop
    // 3. Waiting until all threads complete their job
    // 4. Clear threads

    while(!tasks.empty()){
        // return the resource to OS
        std::this_thread::yield();
    }
    avail = false;
    for(auto &thread:threads){
        thread.join();
    }
    threads.clear();
}

void ThreadPool::addTask(Task *task){
    // Add task into thread pool
    // Using lock to prevent race condition

    std::lock_guard<std::mutex> guard(lock);
    tasks.push_back(task);
}

Task *ThreadPool::getTask(){
    // Get task from task list
    // Using lock to prevent race condition
    // Return Task* | nullptr if no task in list

    std::lock_guard<std::mutex> guard(lock);
    if(tasks.empty()){
        return nullptr;
    }
    Task *task = tasks.front();
    tasks.pop_front();
    return task;
}