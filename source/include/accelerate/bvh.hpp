#pragma once

#include <vector>
#include "shape/triangle.hpp"
#include "shape/shape.hpp"
#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"

// For binary tree node when building BVH
struct BVHTreeNode {
    Bound bound;
    size_t start, triangle_cnt;
    BVHTreeNode *left = nullptr, *right = nullptr;
    int depth;
    int split_axis;
};

// For flatten nodes
struct alignas(32) BVHNode {
    Bound bound;
    union {
        int triangle_idx; // for leaf node
        int right_idx;    // for tree node
    };
    uint16_t triangle_cnt;
    uint8_t depth;
    uint8_t split_axis;
};

struct BVHState {
    size_t total_node_cnt = {};
    size_t leaf_node_cnt = {};
    size_t max_leaf_node_triangle_cnt = {};

    void addLeaf(BVHTreeNode *node, size_t leaf_cnt){
        leaf_node_cnt++;
        max_leaf_node_triangle_cnt = glm::max(max_leaf_node_triangle_cnt, leaf_cnt);
    }
};

class BVHTreeNodeAllocator {
public:
    BVHTreeNodeAllocator(): ptr(4096) {}
    
    BVHTreeNode *allocate(){
        if(ptr == 4096){
            tree_nodes.push_back(new BVHTreeNode[4096]);
            ptr = 0;
        }
        return &(tree_nodes.back()[ptr++]);
    }

    ~BVHTreeNodeAllocator() {
        for(auto e:tree_nodes){
            delete[] e;
        }
        tree_nodes.clear();
    }
private:
    std::vector<BVHTreeNode*> tree_nodes;
    size_t ptr;
};

class BVH : Shape {
public:
    void build(std::vector<Triangle> &&triangles);
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
private:
    void recursive_build(BVHTreeNode *node, size_t start, size_t end, BVHState &state, std::vector<Triangle> &triangles);
    void TreeNodeFlatten(BVHTreeNode *node);

    BVHTreeNodeAllocator treenode_allocator;
    std::vector<BVHNode> flatten_nodes;
    std::vector<Triangle> ordered_triangles;
};
