#pragma once

#include <thread>
#include <atomic>

class SpinLock {
    // atomic_flag is atomic flag in c++
    // flag.set():   set the flag to true
    // flag.clear(): set the flag to false
    // flag = true  -> someone is using this lock
    // flag = false -> lock is free now
    // Using memory_order_... to prevent machine code reordering (before lock or after release)
public:
    void acquire() { while(flag.test_and_set(std::memory_order_acquire)) { std::this_thread::yield(); } };
    void release() { flag.clear(std::memory_order_release); };
private:
    std::atomic_flag flag {};
};

class Guard {
public:
    Guard(SpinLock &spin_lock): spin_lock(spin_lock) { spin_lock.acquire(); };
    ~Guard() { spin_lock.release(); };

    // Prevent Grard being copy
    Guard(const Guard&) = delete;
    Guard& operator=(const Guard&) = delete;
private:
    SpinLock &spin_lock;
};