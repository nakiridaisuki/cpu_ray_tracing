#pragma once

#include "material/base.hpp"

class DielectricMaterial : public BaseMaterial {
public:
    DielectricMaterial(const float ior, const glm::vec3 &albedo_i, const glm::vec3 &albedo_t): 
        ior(ior), albedo_r(albedo_i), albedo_t(albedo_t) {}
    DielectricMaterial(const float ior, const glm::vec3 &albedo): 
        ior(ior), albedo_r(albedo), albedo_t(albedo) {}
    glm::vec3 sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, glm::vec3 &beta, Random &rng) const override;
private:
    float ior;
    glm::vec3 albedo_r, albedo_t;
};