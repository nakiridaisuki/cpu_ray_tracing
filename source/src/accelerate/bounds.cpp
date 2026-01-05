#include "accelerate/bounds.hpp"
#include <algorithm>

void Bound::extend(const glm::vec3 &point) {
    b_min = glm::min(b_min, point);
    b_max = glm::max(b_max, point);
}

void Bound::extend(const Triangle &tri) {
    extend(tri.p1);
    extend(tri.p2);
    extend(tri.p3);
}

bool Bound::intersect(const Ray &ray, float t_min, float t_max) const {
    glm::vec3 t1 = (b_min - ray.origin) / ray.direction;
    glm::vec3 t2 = (b_max - ray.origin) / ray.direction;

    glm::vec3 t_small = glm::min(t1, t2);
    glm::vec3 t_large = glm::max(t1, t2);

    float near = std::max({t_small.x, t_small.y, t_small.z});
    float far  = std::min({t_large.x, t_large.y, t_large.z});

    return near <= far && !(near > t_max || far < t_min);
}