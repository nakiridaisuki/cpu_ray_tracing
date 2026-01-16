#include "shape/scene.hpp"
#include <glm/ext/matrix_transform.hpp>

#include <iostream>

void Scene::addInstance(Shape &shape, const BaseMaterial *material, const glm::vec3 &location, const glm::vec3 &scale, const glm::vec3 &rotate) {
    glm::mat4 world_from_object = (
        glm::translate(glm::mat4(1.f), location) * 
        glm::rotate(glm::mat4(1.f), glm::radians(rotate.z), {0, 0, 1}) * 
        glm::rotate(glm::mat4(1.f), glm::radians(rotate.y), {0, 1, 0}) * 
        glm::rotate(glm::mat4(1.f), glm::radians(rotate.x), {1, 0, 0}) * 
        glm::scale(glm::mat4(1.f), scale)
    );
    glm::mat4 object_from_world = glm::inverse(world_from_object);
    
    instances.push_back(ShapeInstance { &shape, material, world_from_object, object_from_world });
}

std::optional<HitInfo> Scene::intersect(const Ray &ray, float t_min, float t_max) const {
    return scene_bvh.intersect(ray, t_min, t_max);
}