#include "camera/film.hpp"
#include "thread/thread_pool.hpp"
#include "utils/rgb.hpp"
#include <fstream>
#include <iostream>
#include <numeric>

#include "utils/profile.hpp"

void Film::save(const std::filesystem::path &filename) {
    // Using PPM format
    // P6
    // width height
    // 255
    // ...data...

    std::ofstream file(filename, std::ios::binary);
    
    if(!file) {
        throw std::runtime_error("Couldn't open file for writing: " + filename.string());
    }

    file << "P6\n" << width << " " << height << "\n255\n";

    std::vector<uint8_t> buffer;

    buffer.resize(width * height * 3);

    thread_pool.ParallelFor(width, height, [&](size_t x, size_t y){
        RGB rgb(getPixelColor(x, y));
    
        const size_t idx = x + y*width;
        buffer[idx*3] = rgb.r;
        buffer[idx*3 + 1] = rgb.g;
        buffer[idx*3 + 2] = rgb.b;
    }, false);
    thread_pool.wait();

    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}