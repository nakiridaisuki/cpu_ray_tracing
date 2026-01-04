#pragma once

#include "thread/spin_lock.hpp"
#include <iostream>
#include <string>

class Progress {
public:
    Progress(size_t total_work, int step = 1, int width = 50): 
        total_work(total_work), 
        step(step),
        width(width),
        current_work(0), 
        current_percentage(0)
    {}

    void update(size_t works);

private:
    size_t total_work, current_work;
    int step, width, current_percentage;
    SpinLock spin_lock;
};