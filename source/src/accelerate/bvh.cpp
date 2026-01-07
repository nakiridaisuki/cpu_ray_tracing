#include "accelerate/bvh.hpp"
#include "utils/debug_macro.hpp"
#include "utils/profile.hpp"
#include <algorithm>
#include <iostream>

#define MAX_STACK_SIZE 64

void BVH::build(std::vector<Triangle> &&triangles) {
    BVHTreeNode *root = treenode_allocator.allocate();

    for(size_t i=0; i<triangles.size(); i++){
        root->bound.extend(triangles[i]);
    }
    root->depth = 1;

    BVHState state = {};
    recursive_build(root, 0, triangles.size(), state, triangles);

    ordered_triangles = std::move(triangles);
    flatten_nodes.reserve(state.total_node_cnt);
    TreeNodeFlatten(root);

    DEBUG_LINE(std::cout << "BVH state: total nodes " << state.total_node_cnt << std::endl)
    DEBUG_LINE(std::cout << "BVH state: leaf nodes " << state.leaf_node_cnt << std::endl)
    DEBUG_LINE(std::cout << "BVH state: triangles " << ordered_triangles.size() << std::endl)
    DEBUG_LINE(std::cout << "BVH state: Mean leaf node triangle " << static_cast<float>(ordered_triangles.size()) / static_cast<float>(state.leaf_node_cnt) << std::endl)
    DEBUG_LINE(std::cout << "BVH state: Maximum leaf node triangle cnt " << state.max_leaf_node_triangle_cnt << std::endl)
}

void BVH::recursive_build(BVHTreeNode *node, size_t start, size_t end, BVHState &state, std::vector<Triangle> &triangles) {
    state.total_node_cnt++;
    node->start = start;
    node->triangle_cnt = end - start;
    // Split triangles into binary tree
    if(end - start == 1 || node->depth >= MAX_STACK_SIZE){
        state.addLeaf(node, end - start);
        return;
    }

    #define BUCKET_NUM 16
    auto diag = node->bound.diagonal();
    float min_cost = node->bound.area() * (end - start);
    glm::vec3 scale = static_cast<float>(BUCKET_NUM) / diag;
    
    size_t split_bucket_idx = 0;
    Bound min_left_bnd, min_right_bnd;
    size_t min_left_tri_cnt, min_right_tri_cnt;

    for(size_t axis=0; axis<3; axis++){
        std::array<Bound, BUCKET_NUM> bucket_bound, prefix_bound, postfix_bound;
        std::array<size_t, BUCKET_NUM> bucket_tri_cnt{}, prefix_tri_cnt{}, postfix_tri_cnt{};

        for(size_t tri_idx = start; tri_idx < end; tri_idx++){
            auto &tri = triangles[tri_idx];
            float mid = tri.centroid[axis];
            size_t idx = glm::clamp<size_t>((mid - node->bound.b_min[axis]) * scale[axis], 0, BUCKET_NUM - 1);
            bucket_tri_cnt[idx]++;
            bucket_bound[idx].extend(tri);
        }

        for(size_t i=1; i<BUCKET_NUM; i++){
            prefix_bound[i] = prefix_bound[i-1];
            prefix_bound[i].extend(bucket_bound[i-1]);
            prefix_tri_cnt[i] = bucket_tri_cnt[i-1] + prefix_tri_cnt[i-1];
        }
        for(size_t i=BUCKET_NUM-1; i>0; i--){
            postfix_bound[i] = bucket_bound[i];
            postfix_tri_cnt[i] = bucket_tri_cnt[i];
            if(i < BUCKET_NUM - 1){
                postfix_bound[i].extend(postfix_bound[i+1]);
                postfix_tri_cnt[i] += postfix_tri_cnt[i+1];
            }
        }
        
        for(size_t i=1; i<BUCKET_NUM; i++){
            Bound &left_bound = prefix_bound[i];
            size_t left_tri_cnt = prefix_tri_cnt[i];
            
            Bound &right_bound = postfix_bound[i];
            size_t right_tri_cnt = postfix_tri_cnt[i];
            
            if(right_tri_cnt == 0){
                break;
            }
            if(left_tri_cnt != 0){
                float cost = left_bound.area() * left_tri_cnt + \
                right_bound.area() * right_tri_cnt;
                if(cost < min_cost){
                    min_cost = cost;
                    node->split_axis = axis;
                    split_bucket_idx = i;
                    min_left_bnd = left_bound;
                    min_right_bnd = right_bound;
                    min_left_tri_cnt = left_tri_cnt;
                    min_right_tri_cnt = right_tri_cnt;
                }
            }
        }
    }

    if(split_bucket_idx == 0){
        state.addLeaf(node, end - start);
        return;
    }

    node->triangle_cnt = 0;
    
    node->left = treenode_allocator.allocate();
    node->right = treenode_allocator.allocate();

    node->left->depth = node->depth + 1;
    node->right->depth = node->depth + 1;

    node->left->bound = min_left_bnd;
    node->right->bound = min_right_bnd;

    size_t mid_idx =  std::partition(triangles.begin() + start, triangles.begin() + end, [&](auto &tri){
        float mid = tri.centroid[node->split_axis];
        size_t idx = glm::clamp<size_t>((mid - node->bound.b_min[node->split_axis]) * scale[node->split_axis], 0, BUCKET_NUM - 1);
        return idx < split_bucket_idx;
    }) - triangles.begin();

    recursive_build(node->left, start, mid_idx, state, triangles);
    recursive_build(node->right, mid_idx, end, state, triangles);
}

std::optional<HitInfo> BVH::intersect(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closest_hit_info = {};

    // Manual handling the stack
    std::array<int, MAX_STACK_SIZE> stack;
    auto ptr = stack.begin();
    size_t current_node_idx = 0;

    DEBUG_LINE(size_t bound_test_cnt = 0)
    DEBUG_LINE(size_t triangle_test_cnt = 0)

    while(true){
        DEBUG_LINE(bound_test_cnt++)

        auto &node = flatten_nodes[current_node_idx];
        // 1. Check if intersect with this node
        if(!node.bound.intersect(ray, t_min, t_max)){
            if(ptr == stack.begin()) break;
            current_node_idx = *(--ptr);
            continue;
        }
        
        // 2. If has intersection, checking child node recursively
        // Using split axis to desice which side visit first
        if(node.triangle_cnt == 0){
            if(ray.direction[node.split_axis] > 0){
                current_node_idx++;
                *(ptr++) = node.right_idx;
            }
            else{
                *(ptr++) = current_node_idx + 1;
                current_node_idx = node.right_idx;
            }
            continue;
        }
        
        // 3. If this node is leaf, check intersect of it's all triangles
        if(closest_hit_info.has_value()){
            t_max = closest_hit_info->t;
        }
        
        DEBUG_LINE(triangle_test_cnt += node.triangle_cnt)
        for(int i=0; i<node.triangle_cnt; i++){
            auto hit_info = ordered_triangles[node.triangle_idx + i].intersect(ray, t_min, t_max);
            if(hit_info.has_value()){
                t_max = hit_info->t;
                closest_hit_info = hit_info;

                DEBUG_LINE(closest_hit_info->bound_depth = node.depth)
            }
        }        
        
        if(ptr == stack.begin()) break;
        current_node_idx = *(--ptr);
    }

    #ifdef WITH_DEBUG_INFO
    if(closest_hit_info.has_value()){
        closest_hit_info->triangle_test_count = triangle_test_cnt;
        closest_hit_info->bound_test_count = bound_test_cnt;
    }
    #endif

    return closest_hit_info;
}

void BVH::TreeNodeFlatten(BVHTreeNode *node) {
    // Using DFS to flatten the binary tree
    // left node is always next to self node
    // record right node index in flatten node array

    size_t my_idx = flatten_nodes.size();
    flatten_nodes.push_back({});

    if(node->triangle_cnt == 0){
        TreeNodeFlatten(node->left);
        flatten_nodes[my_idx].right_idx = static_cast<int>(flatten_nodes.size());
        TreeNodeFlatten(node->right);
    }
    else{
        flatten_nodes[my_idx].triangle_idx = node->start;
    }
    
    flatten_nodes[my_idx].bound = std::move(node->bound);
    flatten_nodes[my_idx].triangle_cnt = static_cast<uint16_t>(node->triangle_cnt);
    flatten_nodes[my_idx].depth = node->depth;
    flatten_nodes[my_idx].split_axis = node->split_axis;
}