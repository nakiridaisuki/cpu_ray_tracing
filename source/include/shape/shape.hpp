#pragma once

#include "camera/ray.hpp"
#include "accelerate/bounds.hpp"
#include <optional>

struct Shape{
    // An abstrac struct of shape, every shape need to implement intersect
    virtual std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const = 0;
    virtual Bound getBound() const { return {}; }
};
