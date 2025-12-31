#include "camera.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(Film &film, const glm::vec3 &pos, const glm::vec3 &viewpoint, float fovy)
    : film(film), pos(pos) {
    camera_from_clip = glm::inverse(glm::perspective(
        glm::radians(fovy),
        static_cast<float>(film.getWidth()) / static_cast<float>(film.getHeight()),
        1.f, 2.f
    ));
    world_from_camera = glm::inverse(glm::lookAt(pos, viewpoint, {0, 1, 0}));
}

Ray Camera::generateRay(const glm::ivec2 &pixel_corod, const glm::vec2 &offset) const {
    // From film to NDC [0, width] -> [0, 1]
    glm::vec2 ndc = (glm::vec2(pixel_corod) + offset) / glm::vec2(film.getWidth(), film.getHeight());
    // reverse y axis
    ndc.y = 1.f - ndc.y;

    // ndc is [-1, 1], so [0, 1] -> [-1, 1]
    ndc = ndc * 2.f - 1.f;

    // clip: {x, y, 0, near} -> ndc: {x / near, y / near, 0}
    // near = 1
    glm::vec4 clip = {ndc, 0, 1};
    glm::vec3 world = world_from_camera * camera_from_clip * clip;
    return Ray {
        pos,
        glm::normalize(world - pos)
    };
}