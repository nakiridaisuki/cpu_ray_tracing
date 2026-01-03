#include "triangle.hpp"

#include <iostream>

std::optional<HitInfo> Triangle::intersect(const Ray &ray, float t_min, float t_max) const {
    // Hit equation:
    // h = (1-u-v)*p1 + u*p2 + v*p3
    // means the hit point lies inside the triangle
    // > h: hit point
    // > px: three vertices of triangle

    // h = o + t*d
    // => o + t*d = (1-u-v)*p1 + u*p2 + v*p3
    // => (o - p1) = u*(p2 - p1) + v*(p3 - p1) - t*d

    // Let
    // s = (o - p1)
    // e1 = (p2 - p1)
    // e2 = (p3 - p1)
    // this is a linear equation: [e1, e2, -d] * [u, v, t]^T = s

    // Using Cramer's Rule can solve it
    // Let A = [e1, e2, -d]
    // u = det([s, e2, -d]) / det(A)
    // v = det([e1, s, -d]) / det(A)
    // t = det([e1, e2, s]) / det(A)
    // > det([v1, v2, v3]) = (v1 x v2) . v3
    // > Any swap need change of sign

    // Let:
    // s1 = d x e2
    // s2 = s x e1
    // then:
    // det(A) = s1 . e1
    // u = (s1 . s)  / det(A)
    // v = (s2 . d)  / det(A)
    // t = (s1 . e2) / det(A)
    // where u in [0, 1], v in [0, 1], u+v in [0, 1]
    // if det(A) = 0 -> no solution

    glm::vec3 e1 = p2 - p1;
    glm::vec3 e2 = p3 - p1;
    glm::vec3 s = ray.origin - p1;

    glm::vec3 s1 = glm::cross(ray.direction, e2);
    
    float det = glm::dot(s1, e1);
    float eps = 1e-5;
    
    if(std::abs(det) < eps){ return {}; }
    float inv_det = 1.f / det;
    
    float u = glm::dot(s1, s) * inv_det;
    if(u < 0 || 1 < u){ return {}; }

    glm::vec3 s2 = glm::cross(s, e1);
    float v = glm::dot(s2, ray.direction) * inv_det;
    if(v < 0 || u+v > 1){ return {}; }

    float t = glm::dot(s2, e2) * inv_det;
    if(t_min < t && t < t_max){
        glm::vec3 hit_point = ray.hit(t);
        glm::vec3 normal = (1.f-u-v)*n1 + u*n2 + v*n3;
        return HitInfo {t, hit_point, glm::normalize(normal)};
    }
    return {};
}