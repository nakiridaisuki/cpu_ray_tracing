#pragma once

#include <chrono>
#include <string>

class Timer {
public:
    Timer() {
        start();
        end();
    }

    void start() { start_time = std::chrono::steady_clock::now(); }
    void end() { end_time = std::chrono::steady_clock::now(); }
    std::chrono::nanoseconds time() { return end_time - start_time; }

    std::string duration();
private:
    std::chrono::steady_clock::time_point start_time, end_time;
};