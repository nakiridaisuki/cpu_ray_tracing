#pragma once

#include "shape.hpp"

struct ShapeInstance{
    ShapeInstance(Shape *shape, glm::mat4 world_from_object, glm::mat4 object_from_world):
        shape(shape),
        world_from_object(world_from_object),
        object_from_world(object_from_world) {}

    Shape *shape;
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
        Shape &shape, 
        const glm::vec3 &location = {0, 0, 0}, 
        const glm::vec3 &scale = {1, 1, 1}, 
        const glm::vec3 &rotate = {0, 0, 0}
    );
private:
    std::vector<ShapeInstance> instances;
};