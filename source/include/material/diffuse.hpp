#pragma once

#include "material/base.hpp"

class DiffuseMaterial : public BaseMaterial {
public:
    DiffuseMaterial(const glm::vec3 &albedo): albedo(albedo) {}
    glm::vec3 sampleBRDF(const glm::vec3 &view_direction, glm::vec3 &beta, Random &rng) const override;
private:
    glm::vec3 albedo;
};