#include "triangle.hpp"

#include <iostream>

std::optional<HitInfo> Triangle::intersect(const Ray &ray, float t_min, float t_max) const {
    glm::vec3 e1 = p2 - p1;
    glm::vec3 e2 = p3 - p1;
    glm::vec3 s = ray.origin - p1;

    glm::vec3 s1 = glm::cross(ray.direction, e2);
    
    float det = glm::dot(s1, e1);
    float eps = 1e-5;
    
    if(std::abs(det) < eps){ return {}; }
    float inv_det = 1.f / det;
    
    glm::vec3 s2 = glm::cross(s, e1);
    float u = glm::dot(s1, s) * inv_det;
    if(u < 0 || 1 < u){ return {}; }
    float v = glm::dot(s2, ray.direction) * inv_det;
    if(v < 0 || 1 < v){ return {}; }
    
    if(u + v > 1){ return {}; }

    float t = glm::dot(s2, e2) * inv_det;
    if(t_min < t && t < t_max){
        glm::vec3 hit_point = ray.hit(t);
        glm::vec3 normal = (1.f-u-v)*n1 + u*n2 + v*n3;
        return HitInfo {t, hit_point, glm::normalize(normal)};
    }
    return {};
}