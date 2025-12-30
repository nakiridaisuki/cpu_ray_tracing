#pragma once

#include <vector>
#include <thread>
#include <list>
#include <functional>
#include "spin_lock.hpp"

class Task {
public:
    virtual void run() = 0;
};

class ThreadPool {
public:
    static void WorkerThread(ThreadPool *master);
    ThreadPool(size_t thread_count = 0);
    ~ThreadPool();

    void wait() const;
    void addTask(Task *task);
    Task *getTask();

    void ParallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &lambda);
private:
    std::atomic<bool> avail; // for subproccess to stop getting task
    std::atomic<int> uncomplete_tasks; // number of uncomplete tasks
    std::vector<std::thread> threads;
    std::list<Task*> tasks;
    mutable SpinLock spin_lock;
};