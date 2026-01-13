#pragma once

#include <glm/glm.hpp>
#include "camera/ray.hpp"

struct Bound {
    glm::vec3 b_min = glm::vec3(std::numeric_limits<float>::infinity());
    glm::vec3 b_max = glm::vec3(-std::numeric_limits<float>::infinity());

    void extend(const glm::vec3 &point);
    void extend(const Bound &bnd);

    bool intersect(const Ray &ray, glm::vec3 &inv_direc, float t_min, float t_max) const ;
    glm::vec3 diagonal() { return b_max - b_min; }
    float area() {
        glm::vec3 diag = diagonal();
        return (diag.x * (diag.y + diag.z) + diag.y * diag.z) * 2.f;
    }

    glm::vec3 getCorner(size_t idx) const {
        auto coner = b_min;
        if((idx & 0b1) == 0)    coner.x = b_max.x;
        if((idx & 0b10) == 0)   coner.y = b_max.y;
        if((idx & 0b100) == 0)  coner.z = b_max.z;
        return coner;
    }

    bool isVaild() const {
        return b_min.x <= b_max.x && b_min.y <= b_max.y && b_min.z <= b_max.z;
    }
};
