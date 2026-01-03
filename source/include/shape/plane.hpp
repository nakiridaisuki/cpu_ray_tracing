#pragma once

#include "shape/shape.hpp"

struct Plane : public Shape{
    Plane(glm::vec3 point, glm::vec3 normal): point(point), normal(normal) {}

    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;

    glm::vec3 point;
    glm::vec3 normal;
};
