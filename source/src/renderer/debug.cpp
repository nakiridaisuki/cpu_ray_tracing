#include "renderer/debug.hpp"
#include "utils/rgb.hpp"

glm::vec3 TriangleTestCountRenderer::renderPixel(const glm::ivec2 &pixel_coord) {
    #ifdef WITH_DEBUG_INFO
    auto ray = camera.generateRay(pixel_coord);
    auto hit_info = scene.intersect(ray);

    return RGB::GenerateHeatmapRGB(static_cast<float>(ray.triangle_test_count) / 7.f);
    return {};
    #else
    retrurn {};
    #endif
}

glm::vec3 BoundTestCountRenderer::renderPixel(const glm::ivec2 &pixel_coord) {
    #ifdef WITH_DEBUG_INFO
    auto ray = camera.generateRay(pixel_coord);
    auto hit_info = scene.intersect(ray);

    return RGB::GenerateHeatmapRGB(static_cast<float>(ray.bound_test_count) / 200.f);
    return {};
    #else
    retrurn {};
    #endif
}