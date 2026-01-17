#pragma once

#include "material/base.hpp"

class SpecularMaterial : public BaseMaterial {
public:
    SpecularMaterial(const glm::vec3 &albedo): albedo(albedo) {}
    glm::vec3 sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, glm::vec3 &beta, Random &rng) const override;
private:
    glm::vec3 albedo;
};