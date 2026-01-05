#include "accelerate/bvh.hpp"
#include <iostream>

void BVH::build(std::vector<Triangle> &&triangles) {
    BVHTreeNode *root = new BVHTreeNode;
    root->triangles = std::move(triangles);
    root->depth = 1;
    root->updateBound();
    recursive_build(root);
    TreeNodeFlatten(root);
}

void BVH::recursive_build(BVHTreeNode *node) {
    if(node->triangles.size() == 1){
        return;
    }

    glm::vec3 diag = node->bound.diagonal();
    // 0: x, 1: y, 2: z axis
    int max_axis = diag.x > diag.y ? (diag.x > diag.z ? 0 : 2) : (diag.y > diag.z ? 1 : 2);
    float saperate = (node->bound.b_max[max_axis] + node->bound.b_min[max_axis]) / 2;

    std::vector<Triangle> left_tris, right_tris;

    for(auto &tri : node->triangles){
        glm::vec3 mid = (tri.p1 + tri.p2 + tri.p3) / 3.f;
        if(mid[max_axis] < saperate){
            left_tris.push_back(tri);
        }
        else{
            right_tris.push_back(tri);
        }
    }
    
    if(left_tris.empty() || right_tris.empty()){
        return;
    }
    
    node->triangles.clear();

    node->left = new BVHTreeNode();
    node->left->triangles = std::move(left_tris);
    node->left->updateBound();
    node->left->depth = node->depth + 1;
    recursive_build(node->left);

    node->right = new BVHTreeNode();
    node->right->triangles = std::move(right_tris);
    node->right->updateBound();
    node->right->depth = node->depth + 1;
    recursive_build(node->right);
}

std::optional<HitInfo> BVH::intersect(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closest_hit_info = {};

    std::array<int, 64> stack;
    auto ptr = stack.begin();
    size_t current_node_idx = 0;

    *(ptr++) = 0;
    while(ptr != stack.begin()){
        auto &node = flatten_nodes[current_node_idx];
        if(!node.bound.intersect(ray, t_min, t_max)){
            current_node_idx = *(--ptr);
            continue;
        }
    
        if(node.triangle_cnt == 0){
            current_node_idx++;
            *(ptr++) = node.right_idx;
            continue;
        }
        
        if(closest_hit_info.has_value()){
            t_max = closest_hit_info->t;
        }
    
        for(int i=0; i<node.triangle_cnt; i++){
            auto hit_info = ordered_triangles[node.triangle_idx + i].intersect(ray, t_min, t_max);
            if(hit_info.has_value()){
                t_max = hit_info->t;
                closest_hit_info = hit_info;
            }
        }

        current_node_idx = *(--ptr);
    }

    return closest_hit_info;
}

void BVH::TreeNodeFlatten(BVHTreeNode *node) {
    int my_idx = flatten_nodes.size();
    flatten_nodes.push_back({});

    if(node->triangles.empty()){
        TreeNodeFlatten(node->left);
        flatten_nodes[my_idx].right_idx = static_cast<int>(flatten_nodes.size());
        TreeNodeFlatten(node->right);
    }
    else{
        flatten_nodes[my_idx].triangle_idx = static_cast<int>(ordered_triangles.size());
    }
    
    flatten_nodes[my_idx].bound = node->bound;
    flatten_nodes[my_idx].triangle_cnt = static_cast<int>(node->triangles.size());
    flatten_nodes[my_idx].depth = node->depth;
    for(auto &tri:node->triangles){
        ordered_triangles.emplace_back(tri);
    }
}