#include "film.hpp"
#include <fstream>

Film::Film(int width, int height): width(width), height(height) {
    pixels.resize(width * height);
}

void Film::save(const std::filesystem::path &filename) {
    // Using PPM format
    // P3
    // width height
    // 255
    // ...data...

    std::ofstream file(filename);
    file << "P3\n" << width << ' ' << height << "\n255\n";

    for(size_t x=0; x<height; x++){
        for(size_t y=0; y<width; y++){
            const glm::vec3 &color = getPixel(x, y);
            glm::ivec3 color_i = glm::clamp(color * 255.f, 0.f, 255.f);
            file << color_i.x << " " << color_i.y << " " << color_i.z << '\n';
        }
    }
}