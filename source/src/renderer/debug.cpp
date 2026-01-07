#include "renderer/debug.hpp"
#include "utils/rgb.hpp"

glm::vec3 BVHDepthRenderer::renderPixel(const glm::ivec2 &pixel_coord) {
    #ifdef WITH_DEBUG_INFO
    auto ray = camera.generateRay(pixel_coord);
    auto hit_info = scene.intersect(ray);

    if(hit_info.has_value()){
        return RGB::GenerateHeatmapRGB(static_cast<float>(hit_info->bound_depth) / 32.f);
    }
    return {};
    #else
    retrurn {};
    #endif
}

glm::vec3 TriangleTestCountRenderer::renderPixel(const glm::ivec2 &pixel_coord) {
    #ifdef WITH_DEBUG_INFO
    auto ray = camera.generateRay(pixel_coord);
    auto hit_info = scene.intersect(ray);

    if(hit_info.has_value()){
        return RGB::GenerateHeatmapRGB(static_cast<float>(hit_info->triangle_test_count) / 7.f);
    }
    return {};
    #else
    retrurn {};
    #endif
}

glm::vec3 BoundTestCountRenderer::renderPixel(const glm::ivec2 &pixel_coord) {
    #ifdef WITH_DEBUG_INFO
    auto ray = camera.generateRay(pixel_coord);
    auto hit_info = scene.intersect(ray);

    if(hit_info.has_value()){
        return RGB::GenerateHeatmapRGB(static_cast<float>(hit_info->bound_test_count) / 150.f);
    }
    return {};
    #else
    retrurn {};
    #endif
}