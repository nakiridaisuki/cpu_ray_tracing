#include "scene.hpp"
#include <glm/ext/matrix_transform.hpp>

void Scene::addInstance(Shape &shape, const glm::vec3 &location, const glm::vec3 &scale, const glm::vec3 &rotate) {
    glm::mat4 world_from_object = (
        glm::translate(glm::mat4(1.f), location) * 
        glm::rotate(glm::mat4(1.f), glm::radians(rotate.z), {0, 0, 1}) * 
        glm::rotate(glm::mat4(1.f), glm::radians(rotate.y), {0, 1, 0}) * 
        glm::rotate(glm::mat4(1.f), glm::radians(rotate.x), {1, 0, 0}) * 
        glm::scale(glm::mat4(1.f), scale)
    );
    glm::mat4 object_from_world = glm::inverse(world_from_object);
    
    instances.push_back(ShapeInstance {&shape, world_from_object, object_from_world});
}

std::optional<HitInfo> Scene::intersect(const Ray &ray, float t_min, float t_max) const {

    std::optional<HitInfo> closest_hit_info {};
    const ShapeInstance *closest_instance = nullptr;

    for(const auto &instance : instances){
        auto ray_object = ray.objectFromWorld(instance.object_from_world);
        auto hit_info = instance.shape->intersect(ray_object, t_min, t_max);
        if(hit_info.has_value()){
            t_max = hit_info->t;
            closest_hit_info = hit_info;
            closest_instance = &instance;
        }
    }

    if(closest_hit_info.has_value()){
        closest_hit_info->hit_point = closest_instance->world_from_object * glm::vec4(closest_hit_info->hit_point, 1);
        closest_hit_info->normal = glm::transpose(closest_instance->object_from_world) * glm::vec4(closest_hit_info->normal, 0);
    }
    return closest_hit_info;
}