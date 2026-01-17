#pragma once

#include "shape/shape.hpp"
#include "material/base.hpp"
#include "accelerate/bounds.hpp"
#include "accelerate/scene_bvh.hpp"

struct Scene : public Shape{    
public:
    std::optional<HitInfo> intersect(
        const Ray &ray, 
        float t_min = 1e-5, 
        float t_max = std::numeric_limits<float>::infinity()
    ) const override;

    void addInstance(
        Shape &shape, 
        const BaseMaterial *material = nullptr,
        const glm::vec3 &location = {0, 0, 0}, 
        const glm::vec3 &scale = {1, 1, 1}, 
        const glm::vec3 &rotate = {0, 0, 0}
    );

    void build() { scene_bvh.build(instances); }
private:
    std::vector<ShapeInstance> instances;
    SceneBVH scene_bvh;
};