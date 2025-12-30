#include <vector>
#include <filesystem>
#include "glm/glm.hpp"

class Film {
public:
    Film(int width, int height);
    void save(const std::filesystem::path &filename);

    size_t getWidth() { return width; }
    size_t getHeight() { return height; }
    glm::vec3 getPixel(size_t row, size_t col) { return pixels[ row*width + col ]; }
    void setPixel(size_t row, size_t col, const glm::vec3 &color) { pixels[ row*width + col ] = color; }
private:
    size_t width, height;
    std::vector<glm::vec3> pixels;
};
