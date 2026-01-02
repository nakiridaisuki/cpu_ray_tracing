#include "sphere.hpp"

std::optional<HitInfo> Sphere::intersect(const Ray &ray, float t_min, float t_max) const {
    // x_v: x is a vector, x: x is a scalar
    // Hit equation:
    // r_v: radius vector
    // co_v: vector from ray origin to center;
    // r_v = (o_v + t * d_v) - co_v
    // => |r_v|^2 = |ho_v - co_v
    glm::vec3 co = ray.origin - center;
    float B = 2.f * glm::dot(ray.direction, co);
    float C = glm::dot(co, co) - radius * radius;
    float delta = B*B - 4*C;
    if(delta < 0){
        return {};
    }

    float root = (-B - glm::sqrt(delta)) * .5;
    if(root < 0){
        root = (-B + glm::sqrt(delta)) * .5;
    }

    if(t_min < root && root < t_max){
        glm::vec3 hit_point = ray.hit(root);
        glm::vec3 normal = glm::normalize(hit_point - normal);
        return HitInfo {root, hit_point, normal};
    }
    return {};
}