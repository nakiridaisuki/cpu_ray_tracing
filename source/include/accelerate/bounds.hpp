#pragma once

#include <glm/glm.hpp>
#include "camera/ray.hpp"

struct Bound{
    glm::vec3 b_min = glm::vec3(std::numeric_limits<float>::infinity());
    glm::vec3 b_max = glm::vec3(-std::numeric_limits<float>::infinity());

    void extend(const glm::vec3 &point);

    bool intersect(const Ray &ray, float t_min, float t_max) const ;
};
