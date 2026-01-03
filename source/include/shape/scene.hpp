#pragma once

#include "shape/shape.hpp"
#include "shape/material.hpp"

struct ShapeInstance {
    const Shape &shape;
    const Material &material;
    glm::mat4 world_from_object;
    glm::mat4 object_from_world;
};


struct Scene : public Shape{    
public:
    std::optional<HitInfo> intersect(
        const Ray &ray, 
        float t_min = 1e-5, 
        float t_max = std::numeric_limits<float>::infinity()
    ) const override;

    void addInstance(
        const Shape &shape, 
        const Material &material,
        const glm::vec3 &location = {0, 0, 0}, 
        const glm::vec3 &scale = {1, 1, 1}, 
        const glm::vec3 &rotate = {0, 0, 0}
    );
private:
    std::vector<ShapeInstance> instances;
};