#include "thread/thread_pool.hpp"
#include "utils/profile.hpp"
#include <iostream>
#include <cmath>

ThreadPool thread_pool;

ThreadPool::ThreadPool(size_t thread_count) {
    // Construct the thread pool and create `thread_count` threads
    // if `thread_count` is 0, then create supported number of threads
    
    avail = true;
    if(thread_count == 0){
        thread_count = std::thread::hardware_concurrency();
    }
    for(size_t i=0; i<thread_count; i++)
    threads.push_back(std::thread(ThreadPool::WorkerThread, this));
    uncomplete_tasks = 0;
}

ThreadPool::~ThreadPool() {
    // Deconstruct the thread pool
    // 1. Waiting until all tasks are taken
    // 2. Set avail to tell all threads to stop
    // 3. Waiting until all threads complete their job
    // 4. Clear threads
    
    wait();
    avail = false;
    for(auto &thread:threads){
        thread.join();
    }
    threads.clear();
}

void ThreadPool::WorkerThread(ThreadPool *master) {
    // Using avail to check if this thread pool is deconstructed
    // if not, then get task and run it.

    while(master->avail) {
        Task *task = master->getTask();
        if(task == nullptr){
            // return the resource to OS
            std::this_thread::yield();
            continue;
        }
        task->run();
        delete task;

        {
            Guard guard(master->spin_lock);
            master->uncomplete_tasks--;
        }
    }
}

void ThreadPool::wait() const {
    // Waiting for all tasks are taken
    while(true){
        // A scope for lock
        {
            Guard guard(spin_lock);
            if(uncomplete_tasks == 0) return;
        }
        // return the resource to OS
        std::this_thread::yield();
    }
}

void ThreadPool::addTask(Task *task){
    // Add task into thread pool
    // Using lock to prevent race condition

    Guard guard(spin_lock);
    uncomplete_tasks++;
    tasks.push(task);
}

Task *ThreadPool::getTask(){
    // Get task from task list
    // Using lock to prevent race condition
    // Return Task* | nullptr if no task in list

    Guard guard(spin_lock);
    if(tasks.empty()){
        return nullptr;
    }
    Task *task = tasks.front();
    tasks.pop();
    return task;
}

class ParallelForTask : public Task {
    // Parallel for task class
    // Accept two argument and a function
public:
    ParallelForTask(size_t x, size_t y, size_t chunk_x, size_t chunk_y, const std::function<void(size_t, size_t)> &lambda)
        : x(x), y(y), chunk_x(chunk_x), chunk_y(chunk_y), lambda(lambda) {}

    void run() override {
        for(int i=x; i<x+chunk_x; i++){
            for(int j=y; j<y+chunk_y; j++){
                lambda(i, j);
            }
        }
    }
private:
    size_t x, y, chunk_x, chunk_y;
    std::function<void(size_t, size_t)> lambda;
};


void ThreadPool::ParallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &lambda, bool is_complex){
    Guard guard(spin_lock);

    float max_task_num;
    if(is_complex){
        max_task_num = std::sqrt(threads.size() * 8);
    }
    else{
        max_task_num = std::sqrt(threads.size() * 1);
    }
    size_t chunk_x = std::ceil(static_cast<float>(width) / max_task_num);
    size_t chunk_y = std::ceil(static_cast<float>(height) / max_task_num);

    for(int x = 0; x < width; x += chunk_x){
        for(int y = 0; y < height; y += chunk_y){
            int wx = chunk_x, wy = chunk_y;
            if(x + wx > width) wx = width - x;
            if(y + wy > height) wy = height - y;
            tasks.push(new ParallelForTask(x, y, wx, wy, lambda));
            uncomplete_tasks ++;
        }
    }
}