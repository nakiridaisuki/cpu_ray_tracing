#pragma once

#include "shape/shape.hpp"

struct Triangle : public Shape{
    glm::vec3 p1, p2, p3;
    glm::vec3 n1, n2, n3;
    glm::vec3 centroid;

    Triangle(glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, glm::vec3 &n1, glm::vec3 &n2, glm::vec3 &n3):
        p1(p1), p2(p2), p3(p3),
        n1(n1), n2(n2), n3(n3) {
            centroid = (p1 + p2 + p3) / 3.f;
    }

    Triangle(glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3):
        p1(p1), p2(p2), p3(p3) {
            glm::vec3 v1 = p2 - p1, v2 = p3 - p1;
            glm::vec3 normal = glm::cross(v1, v2);
            n1 = n2 = n3 = normal;
            centroid = (p1 + p2 + p3) / 3.f;
    }

    Triangle() {}

    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bound getBound() const override {
        Bound bnd = {};
        bnd.extend(p1);
        bnd.extend(p2);
        bnd.extend(p3);
        return bnd;
    }
};
