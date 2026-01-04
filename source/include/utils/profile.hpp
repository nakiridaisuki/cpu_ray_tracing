#pragma once

#include "utils/timer.hpp"

#define PROFILE(name) Profile __profile(name);

struct Profile : public Timer {
    Profile(const std::string &name);
    ~Profile();

    std::string name;
};