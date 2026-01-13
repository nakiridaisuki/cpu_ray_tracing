#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "utils/random.hpp"

inline glm::vec2 UniformSampleUnitDisk(const glm::vec2 &u){
    float r = glm::sqrt(u.x);
    float theta = 2 * glm::pi<float>() * u.y;
    return { r * glm::cos(theta), r * glm::sin(theta) };
}

inline glm::vec3 CosineSampleHemisphere(const glm::vec2 &u){
    float r = glm::sqrt(u.x);
    float theta = 2 * glm::pi<float>() * u.y;
    return { r * glm::cos(theta), glm::sqrt(1 - r*r), r * glm::sin(theta) };
}

inline glm::vec3 UniformSampleHemisphere(Random &uniform){
    glm::vec3 result;
    do {
        result = { uniform.gen(-1, 1), uniform.gen(-1, 1), uniform.gen(-1, 1) };
    } while (glm::length(result) > 1);

    result.y = glm::abs(result.y);
    result = glm::normalize(result);
    return result;
}


 