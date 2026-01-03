#include "shape/plane.hpp"

std::optional<HitInfo> Plane::intersect(const Ray &ray, float t_min, float t_max) const {
    // Hit equation:
    // > Any vertor on the plane must perpendicular to normal
    // (o + t*d - p) . n = 0
    // => t*(d . n) + (o - p).n = 0
    // => t = (p-o).n / (d.n)

    float t = glm::dot((point - ray.origin), normal) / glm::dot(ray.direction, normal);

    if(t_min < t && t < t_max){
        return HitInfo {t, ray.hit(t), normal};
    }

    return {};
}