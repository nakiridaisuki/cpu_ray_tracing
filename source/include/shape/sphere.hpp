#pragma once

#include "shape/shape.hpp"

struct Sphere : public Shape {
    Sphere (const glm::vec3 &center, float radius): center(center), radius(radius) {}
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;

    glm::vec3 center;
    float radius;

    void set_center(const glm::vec3 ns) { center = ns; }

    Bound getBound() const {
        return { center - radius, center + radius };
    }
};
