#pragma once

#include "shape/triangle.hpp"
#include "accelerate/bvh.hpp"
#include <filesystem>

class Model : public Shape{
public:
    Model(std::vector<Triangle> &triangles) { 
        auto ts = triangles;
        bvh.build(std::move(ts)); 
    }
    Model(const std::filesystem::path &filename);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bound getBound() const override { return bvh.getBound(); }
private:
    BVH bvh;
};
