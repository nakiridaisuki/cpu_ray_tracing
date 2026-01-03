#include "utils/frame.hpp"

Frame::Frame(glm::vec3 &normal) {
    y_axis = normal;
    x_axis = normal.y < 0.9 ? glm::cross(normal, {0, 1, 0}) : glm::cross(normal, {1, 0, 0});
    z_axis = glm::cross(x_axis, y_axis);
}

glm::vec3 Frame::worldFromLocal(const glm::vec3 &direction_local) const {
    return glm::normalize(
        direction_local.x * x_axis + 
        direction_local.y * y_axis + 
        direction_local.z * z_axis
    );
}

glm::vec3 Frame::localFromWorld(const glm::vec3 &direction_world) const {
    return glm::normalize(glm::vec3 {
        glm::dot(direction_world, x_axis),
        glm::dot(direction_world, y_axis),
        glm::dot(direction_world, z_axis),
    });
}
