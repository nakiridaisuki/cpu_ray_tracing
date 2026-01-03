#include "thread/thread_pool.hpp"

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
    ParallelForTask(size_t x, size_t y, const std::function<void(size_t, size_t)> lambda)
        : x(x), y(y), lambda(lambda) {}

    void run() override {
        lambda(x, y);
    }
private:
    size_t x, y;
    std::function<void(size_t, size_t)> lambda;
};


void ThreadPool::ParallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &lambda){
    Guard guard(spin_lock);

    for(int x=0; x<width; x++){
        for(int y=0; y<height; y++){
            tasks.push(new ParallelForTask(x, y, lambda));
        }
    }
    uncomplete_tasks += width * height;
}