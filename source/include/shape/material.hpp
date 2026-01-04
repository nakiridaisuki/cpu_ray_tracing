#pragma once

#include <glm/glm.hpp>

struct Material{
    glm::vec3 albedo = {1, 1, 1};
    bool is_specular = false;
    glm::vec3 emissive = {0, 0, 0};

    Material(glm::vec3 color = {1, 1, 1}, bool specular = false, glm::vec3 emit = {0, 0, 0}) 
        : albedo(color), is_specular(specular), emissive(emit) {}
};
