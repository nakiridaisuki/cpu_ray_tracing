#pragma once

#include "shape/triangle.hpp"
#include "accelerate/bounds.hpp"
#include <filesystem>

class Model : public Shape{
public:
    Model(const std::vector<Triangle> &triangles): triangles(triangles) { build(); }
    Model(const std::filesystem::path &filename);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    void build();
private:
    std::vector<Triangle> triangles;
    Bound bound;
};
