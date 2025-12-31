#pragma once

#include <vector>
#include <filesystem>
#include "glm/glm.hpp"

class Film {
public:
    Film(int width, int height);
    void save(const std::filesystem::path &filename);

    size_t getWidth() const { return width; }
    size_t getHeight() const { return height; }
    glm::vec3 getPixel(size_t x, size_t y) { return pixels[ y*width + x ]; }
    void setPixel(size_t x, size_t y, const glm::vec3 &color) { pixels[ y*width + x ] = color; }
private:
    size_t width, height;
    std::vector<glm::vec3> pixels;
};
