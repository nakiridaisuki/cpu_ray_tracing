#pragma once

#include <vector>
#include "shape/triangle.hpp"
#include "shape/shape.hpp"
#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"

struct BVHTreeNode {
    Bound bound;
    std::vector<Triangle> triangles;
    BVHTreeNode *left = nullptr, *right = nullptr;
    int depth;

    void updateBound() {
        for(auto &tri : triangles){
            bound.extend(tri);
        }
    }
};

struct alignas(32) BVHNode {
    Bound bound;
    union {
        int triangle_idx;
        int right_idx;
    };
    uint16_t triangle_cnt;
    uint8_t depth;
};

class BVH : Shape {
public:
    void build(std::vector<Triangle> &&triangles);
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
private:
    void recursive_build(BVHTreeNode *node);
    void TreeNodeFlatten(BVHTreeNode *node);

    std::vector<BVHNode> flatten_nodes;
    std::vector<Triangle> ordered_triangles;
};
