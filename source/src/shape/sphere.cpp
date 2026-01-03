#include "shape/sphere.hpp"

std::optional<HitInfo> Sphere::intersect(const Ray &ray, float t_min, float t_max) const {
    // Hit equation:
    // oh = oc + rv
    // > oh: origin to hit point
    // > oc: origin to center
    // > rv: radius vector
    // => rv = oh - oc

    // oh = t * d (t * direction vector)
    // -oc = co = o - c
    // => rv = t * d + co
    // => |rv|^2 = (t * d + co) . (t * d + co)
    // => (t^2 * d.d) + (2t * d.co) + (co.co - r^2) = 0 (this is an quadratic equation)

    // Let:
    // A = d.d
    // B = 2 * d.co
    // C = co.co - r^2
    // We can solve t using quadratic formula.

    // delta = B*B - 4*A*C
    // root = (-B +- sqrt(delta)) / 2A
    // check the root from near(smaller t aka -) to far(larger t aka +)
    // root need > 0

    glm::vec3 co = ray.origin - center;
    float A = glm::dot(ray.direction, ray.direction);
    float B = 2.f * glm::dot(ray.direction, co);
    float C = glm::dot(co, co) - radius * radius;
    float delta = B*B - 4*A*C;
    if(delta < 0){
        return {};
    }

    float root = (-B - glm::sqrt(delta)) * .5 / A;
    if(root < 0){
        root = (-B + glm::sqrt(delta)) * .5 / A;
    }

    if(t_min < root && root < t_max){
        glm::vec3 hit_point = ray.hit(root);
        glm::vec3 normal = glm::normalize(hit_point - center);
        return HitInfo {root, hit_point, normal};
    }
    return {};
}