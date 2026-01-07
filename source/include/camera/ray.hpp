#pragma once

#include "glm/glm.hpp"
#include "shape/material.hpp"
#include "utils/debug_macro.hpp"

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    glm::vec3 hit(float t) const { return origin + direction * t; };

    Ray objectFromWorld(const glm::mat4 &object_from_world) const;
};

struct HitInfo {
    // A hit info struct
    // t: root of hit equation
    // hit_point: ..
    // normal: ..
    float t;
    glm::vec3 hit_point;
    glm::vec3 normal;
    const Material *material = nullptr;

    DEBUG_LINE(size_t bound_test_count)
    DEBUG_LINE(size_t triangle_test_count)
    DEBUG_LINE(size_t bound_depth)
};