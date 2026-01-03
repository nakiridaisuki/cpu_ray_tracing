#include "camera/film.hpp"
#include "thread/thread_pool.hpp"
#include "utils/rgb.hpp"
#include <fstream>
#include <iostream>
#include <numeric>

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
    const size_t factor = std::gcd(width, height);

    thread_pool.ParallelFor(width/factor, height/factor, [&](size_t x, size_t y){
        for(int i=x*factor; i<(x+1)*factor; i++){
            for(int j=y*factor; j<(y+1)*factor; j++){
                RGB rgb(getPixelColor(i, j));

                const size_t idx = i + j*width;
                buffer[idx*3] = rgb.r;
                buffer[idx*3 + 1] = rgb.g;
                buffer[idx*3 + 2] = rgb.b;
            }
        }
    });
    thread_pool.wait();

    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}