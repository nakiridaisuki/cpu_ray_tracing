#pragma once

#include "ray.hpp"
#include "optional"

struct Shape{
    // An abstrac struct of shape, every shape need to implement intersect
    // t_min, t_max is the range of t (result)
    virtual std::optional<HitInfo> intersect(
        const Ray &ray, 
        float t_min = 1e-5, 
        float t_max = std::numeric_limits<float>::infinity()
    ) const = 0;
};
