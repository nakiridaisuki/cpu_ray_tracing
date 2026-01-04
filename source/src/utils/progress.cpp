#include "utils/progress.hpp"

void Progress::update(size_t works) { 
    Guard guard(spin_lock);
    current_work += works; 

    float percentage = static_cast<float>(current_work) / total_work;
    if(percentage*100 - current_percentage < step) return;
    current_percentage = percentage * 100;

    std::cout << current_percentage << "%" << std::endl;

    // int current_width = percentage * static_cast<float>(width);
    
    // std::string buffer = "\e[?25l[";
    // for(int i=0; i<current_width-1; i++) buffer.push_back('=');
    // buffer.push_back('>');
    // for(int i=current_width; i<width; i++) buffer.push_back(' ');
    // buffer.push_back(']');

    // std::cout << buffer << " " << static_cast<int>(percentage * 100) << "%\r";
    // std::cout.flush();
}