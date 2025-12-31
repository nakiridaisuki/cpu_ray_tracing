#include "sphere.hpp"

std::optional<float> Sphere::intersect(const Ray &ray) const {
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
    if(root > 0){
        return root;
    }
    return {};
}