#include "film.hpp"
#include <fstream>

Film::Film(int width, int height): width(width), height(height) {
    pixels.resize(width * height);
}

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
    buffer.reserve(width * height * 3);
    for(size_t y=0; y<height; y++){
        for(size_t x=0; x<width; x++){
            const glm::vec3 &color = getPixel(x, y);
            glm::u8vec3 color_i = glm::clamp(color * 255.f, 0.f, 255.f);
            buffer.push_back(color_i.x);
            buffer.push_back(color_i.y);
            buffer.push_back(color_i.z);
        }
    }
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}