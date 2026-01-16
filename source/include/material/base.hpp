#pragma once

#include <glm/glm.hpp>
#include "utils/random.hpp"

class BaseMaterial {
public:
    virtual glm::vec3 sampleBRDF(const glm::vec3 &view_direction, glm::vec3 &beta, Random &rng) const = 0;
    void setEmissive(const glm::vec3 &emissive) { this->emissive = emissive; }
    glm::vec3 emissive;
};