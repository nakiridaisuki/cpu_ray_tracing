#include "renderer/simple_rt.hpp"
#include "utils/frame.hpp"

glm::vec3 SimpleRTRenderer::renderPixel(const glm::ivec2 &pixel_coord) {
    auto ray = camera.generateRay(pixel_coord, { uniform.gen(), uniform.gen() });
    glm::vec3 beta = {1, 1, 1};
    glm::vec3 color = {0, 0, 0};

    for(int k=0; k<50; k++){
        auto hit_info = scene.intersect(ray);
        if(hit_info.has_value()){
            color += beta * hit_info->material->emissive;
            beta *= hit_info->material->albedo;

            Frame frame(hit_info->normal);
            glm::vec3 light_direction;
            if(hit_info->material->is_specular){
                glm::vec3 view_direction = frame.localFromWorld(-ray.direction);
                view_direction.x = -view_direction.x;
                view_direction.z = -view_direction.z;
                light_direction = frame.worldFromLocal(view_direction);
            }
            else{
                do {
                    light_direction = { uniform.gen(-1, 1), uniform.gen(-1, 1), uniform.gen(-1, 1) };
                } while (glm::length(light_direction) < 1);
                light_direction.y = glm::abs(light_direction.y);
                light_direction = glm::normalize(light_direction);
                light_direction = frame.worldFromLocal(light_direction);
            }

            ray.direction = light_direction;
            ray.origin = hit_info->hit_point + light_direction * 1e-4f;
        }
        else{
            break;
        }
    }
    return color;
}