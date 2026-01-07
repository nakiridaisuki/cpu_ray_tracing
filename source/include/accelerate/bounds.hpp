#pragma once

#include <glm/glm.hpp>
#include "camera/ray.hpp"
#include "shape/triangle.hpp"

struct Bound{
    glm::vec3 b_min = glm::vec3(std::numeric_limits<float>::infinity());
    glm::vec3 b_max = glm::vec3(-std::numeric_limits<float>::infinity());

    void extend(const glm::vec3 &point);
    void extend(const Triangle &tri);
    void extend(const Bound &bnd);

    bool intersect(const Ray &ray, float t_min, float t_max) const ;
    glm::vec3 diagonal() { return b_max - b_min; }
    float area() {
        glm::vec3 diag = diagonal();
        return (diag.x * (diag.y + diag.z) + diag.y * diag.z) * 2.f;
    }
};
