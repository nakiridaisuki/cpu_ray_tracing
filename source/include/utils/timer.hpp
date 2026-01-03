#pragma once

#include <chrono>

class Timer {
public:
    Timer() {
        start();
        end();
    }

    void start() { start_time = std::chrono::steady_clock::now(); }
    void end() { end_time = std::chrono::steady_clock::now(); }
    std::chrono::nanoseconds duration() { return end_time - start_time; }

    size_t ms() { 
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration()).count();
    }
    size_t mus() {
        duration();
        return std::chrono::duration_cast<std::chrono::microseconds>(duration()).count();
    }
    size_t ns() {
        return duration().count();
    }
private:
    std::chrono::steady_clock::time_point start_time, end_time;
};