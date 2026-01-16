#include "accelerate/scene_bvh.hpp"
#include "utils/debug_macro.hpp"
#include "utils/profile.hpp"
#include <algorithm>
#include <iostream>

#define MAX_STACK_SIZE 64

void SceneBVH::build(std::vector<ShapeInstance> &&instances) {
    SceneBVHTreeNode *root = treenode_allocator.allocate();

    for(auto &instance : instances){
        if(instance.shape->getBound().isVaild()){
            instance.updateBound();
            root->bound.extend(instance.world_bound);
            ordered_instances.push_back(instance);
        }
        else{
            infinity_instances.push_back(instance);
        }
    }
    root->depth = 1;

    SceneBVHState state = {};
    recursive_build(root, 0, instances.size(), state, ordered_instances);

    flatten_nodes.reserve(state.total_node_cnt);
    TreeNodeFlatten(root);

    DEBUG_LINE(std::cout << "SceneBVH state: total nodes " << state.total_node_cnt << std::endl)
    DEBUG_LINE(std::cout << "SceneBVH state: leaf nodes " << state.leaf_node_cnt << std::endl)
    DEBUG_LINE(std::cout << "SceneBVH state: instances " << ordered_instances.size() << std::endl)
    DEBUG_LINE(std::cout << "SceneBVH state: Mean leaf node instance " << static_cast<float>(ordered_instances.size()) / static_cast<float>(state.leaf_node_cnt) << std::endl)
    DEBUG_LINE(std::cout << "SceneBVH state: Maximum leaf node instance cnt " << state.max_leaf_node_instance_cnt << std::endl)
}

// Recursively build the SceneBVH tree
// Using inplace partition of instances list
// Every node control instances[start, end)
void SceneBVH::recursive_build(SceneBVHTreeNode *node, size_t start, size_t end, SceneBVHState &state, std::vector<ShapeInstance> &instances) {
    // Calculate node data
    state.total_node_cnt++;
    node->start = start;
    node->instance_cnt = end - start;

    // Split instances into binary tree
    if(end - start == 1 || node->depth >= MAX_STACK_SIZE){
        state.addLeaf(node, end - start);
        return;
    }

    // All variables used in finding minimum cost of split
    // Avoid using dinamic vector to improve execution time
    #define BUCKET_NUM 12
    auto diag = node->bound.diagonal();
    float min_cost = node->bound.area() * (end - start);

    glm::vec3 scale;
    for(size_t i=0; i<3; i++){
        if(diag[i] > std::numeric_limits<float>::epsilon()){
            scale[i] = static_cast<float>(BUCKET_NUM) / diag[i];
        }
        else{
            scale[i] = 0;
        }
    }
    
    size_t split_bucket_idx = 0;
    Bound min_left_bnd, min_right_bnd;
    size_t min_left_ins_cnt, min_right_ins_cnt;

    // Check BUCKET_NUM different saperation on 3 axis
    // For every axis, calculate all bucket bound and instance number in this bucket
    // Then pre-calculate the prefix and postfix bound box, 
    // and use it to calculate every cutting cost.
    // After find out the minimum cutting bucket index and axis,
    // fill the data into child nodes and inplace partition the instance list.
    for(size_t axis=0; axis<3; axis++){
        std::array<Bound, BUCKET_NUM> bucket_bound, prefix_bound, postfix_bound;
        std::array<size_t, BUCKET_NUM> bucket_ins_cnt{}, prefix_ins_cnt{}, postfix_ins_cnt{};

        // Calcualte bucket data
        for(size_t ins_idx = start; ins_idx < end; ins_idx++){
            auto &ins = instances[ins_idx];
            float mid = ins.center[axis];
            size_t idx = glm::clamp<size_t>((mid - node->bound.b_min[axis]) * scale[axis], 0, BUCKET_NUM - 1);
            bucket_ins_cnt[idx]++;
            bucket_bound[idx].extend(ins.world_bound);
        }

        // Pre-calculate prefix and postfix
        for(size_t i=1; i<BUCKET_NUM; i++){
            prefix_bound[i] = prefix_bound[i-1];
            prefix_bound[i].extend(bucket_bound[i-1]);
            prefix_ins_cnt[i] = bucket_ins_cnt[i-1] + prefix_ins_cnt[i-1];
        }
        for(size_t i=BUCKET_NUM-1; i>0; i--){
            postfix_bound[i] = bucket_bound[i];
            postfix_ins_cnt[i] = bucket_ins_cnt[i];
            if(i < BUCKET_NUM - 1){
                postfix_bound[i].extend(postfix_bound[i+1]);
                postfix_ins_cnt[i] += postfix_ins_cnt[i+1];
            }
        }
        
        // Finding the minimum cutting bucket index
        for(size_t i=1; i<BUCKET_NUM; i++){
            Bound &left_bound = prefix_bound[i];
            size_t left_ins_cnt = prefix_ins_cnt[i];
            
            Bound &right_bound = postfix_bound[i];
            size_t right_ins_cnt = postfix_ins_cnt[i];
            
            if(right_ins_cnt == 0){
                break;
            }
            if(left_ins_cnt != 0){
                float cost = left_bound.area() * left_ins_cnt + \
                right_bound.area() * right_ins_cnt;
                if(cost < min_cost){
                    min_cost = cost;
                    node->split_axis = axis;
                    split_bucket_idx = i;
                    min_left_bnd = left_bound;
                    min_right_bnd = right_bound;
                    min_left_ins_cnt = left_ins_cnt;
                    min_right_ins_cnt = right_ins_cnt;
                }
            }
        }
    }

    // Check if this node can be splited
    if(split_bucket_idx == 0){
        state.addLeaf(node, end - start);
        return;
    }

    // Fill data and build recursively
    node->instance_cnt = 0;
    
    node->left = treenode_allocator.allocate();
    node->right = treenode_allocator.allocate();

    node->left->depth = node->depth + 1;
    node->right->depth = node->depth + 1;

    node->left->bound = min_left_bnd;
    node->right->bound = min_right_bnd;

    // Inplace partition, improve huge execution time
    size_t mid_idx =  std::partition(instances.begin() + start, instances.begin() + end, [&](auto &ins){
        float mid = ins.center[node->split_axis];
        size_t idx = glm::clamp<size_t>((mid - node->bound.b_min[node->split_axis]) * scale[node->split_axis], 0, BUCKET_NUM - 1);
        return idx < split_bucket_idx;
    }) - instances.begin();

    if(mid_idx == start || mid_idx == end){
        state.addLeaf(node, end - start);
        return;
    }

    recursive_build(node->left, start, mid_idx, state, instances);
    recursive_build(node->right, mid_idx, end, state, instances);
}

std::optional<HitInfo> SceneBVH::intersect(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closest_hit_info = {};
    const ShapeInstance *closest_instance = nullptr;

    // Manual handling the stack
    std::array<int, MAX_STACK_SIZE> stack;
    auto ptr = stack.begin();
    size_t current_node_idx = 0;

    DEBUG_LINE(size_t bound_test_cnt = 0)

    glm::vec3 inv_direc = 1.f / ray.direction;
    while(true){
        DEBUG_LINE(bound_test_cnt++)

        auto &node = flatten_nodes[current_node_idx];
        // 1. Check if intersect with this node
        if(!node.bound.intersect(ray, inv_direc, t_min, t_max)){
            if(ptr == stack.begin()) break;
            current_node_idx = *(--ptr);
            continue;
        }
        
        // 2. If has intersection, checking child node recursively
        // Using split axis to desice which side visit first
        if(node.instance_cnt == 0){
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
        
        // 3. If this node is leaf, check intersect of it's all instances
        if(closest_hit_info.has_value()){
            t_max = closest_hit_info->t;
        }
        
        for(int i=0; i<node.instance_cnt; i++){
            auto &instance = ordered_instances[node.instance_idx + i];
            auto ray_object = ray.objectFromWorld(instance.object_from_world);
            auto hit_info = instance.shape->intersect(ray_object, t_min, t_max);

            ray.triangle_test_count += ray_object.triangle_test_count;
            ray.bound_test_count += ray_object.bound_test_count;

            if(hit_info.has_value()){
                t_max = hit_info->t;
                closest_hit_info = hit_info;
                closest_instance = &ordered_instances[node.instance_idx + i];
            }
        }        
        
        if(ptr == stack.begin()) break;
        current_node_idx = *(--ptr);
    }

    for(auto &instance : infinity_instances){
        auto ray_object = ray.objectFromWorld(instance.object_from_world);
        auto hit_info = instance.shape->intersect(ray_object, t_min, t_max);

        ray.triangle_test_count += ray_object.triangle_test_count;
        ray.bound_test_count += ray_object.bound_test_count;

        if(hit_info.has_value()){
            t_max = hit_info->t;
            closest_hit_info = hit_info;
            closest_instance = &instance;
        }
    }

    #ifdef WITH_DEBUG_INFO
    ray.bound_test_count += bound_test_cnt;
    #endif

    if(closest_hit_info.has_value()){
        closest_hit_info->hit_point = closest_instance->world_from_object * glm::vec4(closest_hit_info->hit_point, 1);
        closest_hit_info->normal = glm::normalize(glm::vec3(
            glm::transpose(closest_instance->object_from_world) * glm::vec4(closest_hit_info->normal, 0)
        ));
        closest_hit_info->material = closest_instance->material;
    }

    return closest_hit_info;
}

void SceneBVH::TreeNodeFlatten(SceneBVHTreeNode *node) {
    // Using DFS to flatten the binary tree
    // left node is always next to self node
    // record right node index in flatten node array

    size_t my_idx = flatten_nodes.size();
    flatten_nodes.push_back({});

    if(node->instance_cnt == 0){
        TreeNodeFlatten(node->left);
        flatten_nodes[my_idx].right_idx = static_cast<int>(flatten_nodes.size());
        TreeNodeFlatten(node->right);
    }
    else{
        flatten_nodes[my_idx].instance_idx = node->start;
    }
    
    flatten_nodes[my_idx].bound = std::move(node->bound);
    flatten_nodes[my_idx].instance_cnt = static_cast<uint16_t>(node->instance_cnt);
    flatten_nodes[my_idx].split_axis = node->split_axis;
}