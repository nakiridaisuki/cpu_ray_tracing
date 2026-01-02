#pragma once

#include "glm/glm.hpp"

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    glm::vec3 hit(float t) const { return origin + direction * t; };
};

struct HitInfo {
    // A hit info struct
    // t: root of hit equation
    // hit_point: ..
    // normal: ..
    float t;
    glm::vec3 hit_point;
    glm::vec3 normal;
};