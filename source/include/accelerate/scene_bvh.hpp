#pragma once

#include <vector>
#include "shape/shape.hpp"
#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"

struct ShapeInstance {
    const Shape *shape;
    const BaseMaterial *material;
    glm::mat4 world_from_object;
    glm::mat4 object_from_world;

    Bound world_bound {};
    glm::vec3 center {};

    void updateBound() {
        world_bound = {};
        auto tmp = shape->getBound();
        for(size_t i=0; i<8; i++){
            glm::vec3 world_corner = world_from_object * glm::vec4{ tmp.getCorner(i), 1 };
            world_bound.extend(world_corner);
        }
        center = (world_bound.b_min + world_bound.b_max) * 0.5f;
    }
};

// For binary tree node when building SceneBVH
struct SceneBVHTreeNode {
    Bound bound;
    size_t start, instance_cnt;
    SceneBVHTreeNode *left = nullptr, *right = nullptr;
    int depth;
    int split_axis;
};

// For flatten nodes
struct alignas(32) SceneBVHNode {
    Bound bound;
    union {
        int instance_idx; // for leaf node
        int right_idx;    // for tree node
    };
    uint16_t instance_cnt;
    uint8_t split_axis;
};

struct SceneBVHState {
    size_t total_node_cnt = {};
    size_t leaf_node_cnt = {};
    size_t max_leaf_node_instance_cnt = {};
    size_t max_leaf_node_depth {};

    void addLeaf(SceneBVHTreeNode *node, size_t leaf_cnt){
        leaf_node_cnt++;
        max_leaf_node_instance_cnt = glm::max(max_leaf_node_instance_cnt, leaf_cnt);
        max_leaf_node_depth = node->depth;
    }
};

class SceneBVHTreeNodeAllocator {
public:
    SceneBVHTreeNodeAllocator(): ptr(4096) {}
    
    SceneBVHTreeNode *allocate(){
        if(ptr == 4096){
            tree_nodes.push_back(new SceneBVHTreeNode[4096]);
            ptr = 0;
        }
        return &(tree_nodes.back()[ptr++]);
    }

    ~SceneBVHTreeNodeAllocator() {
        for(auto e:tree_nodes){
            delete[] e;
        }
        tree_nodes.clear();
    }
private:
    std::vector<SceneBVHTreeNode*> tree_nodes;
    size_t ptr;
};

class SceneBVH : Shape {
public:
    void build(std::vector<ShapeInstance> &instances);
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bound getBound() const { return flatten_nodes[0].bound; }
private:
    void recursive_build(SceneBVHTreeNode *node, size_t start, size_t end, SceneBVHState &state, std::vector<ShapeInstance*> &instances);
    void TreeNodeFlatten(SceneBVHTreeNode *node);
    SceneBVHTreeNodeAllocator treenode_allocator;
    std::vector<SceneBVHNode> flatten_nodes;
    std::vector<ShapeInstance*> ordered_instances;
    std::vector<ShapeInstance*> infinity_instances;
};
