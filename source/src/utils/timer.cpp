#include "utils/timer.hpp"

std::string Timer::duration() {
    auto dt = time().count();

    // nano second
    if(dt < 1000){
        return std::to_string(dt) + " ns";
    }
    dt /= 1000;

    // micro second
    if(dt < 1000){
        return std::to_string(dt) + " us";
    }
    dt /= 1000;

    // milli second
    return std::to_string(dt) + " ms";
    // if(dt < 1000){
    // }
    // dt /= 1000;

    // // second
    // if(dt < 60){
    //     return std::to_string(dt) + " sec";
    // }
    // dt /= 60;

    // // minute
    // if(dt < 60){
    //     return std::to_string(dt) + " min";
    // }
    // dt / 60;

    // // hour
    // return std::to_string(dt) + " hr";
}