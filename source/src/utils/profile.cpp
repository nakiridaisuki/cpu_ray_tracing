#include "utils/profile.hpp"
#include <iostream>

Profile::Profile(const std::string &name): name(name) {
    start();
}

Profile::~Profile() {
    end();
    std::cout << "Profile " + name + " cost " + duration() << std::endl;
}
