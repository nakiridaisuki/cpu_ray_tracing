#include "renderer/simple_rt.hpp"
#include "utils/frame.hpp"
#include "utils/random.hpp"

glm::vec3 SimpleRTRenderer::renderPixel(const glm::ivec2 &pixel_coord) {
    auto ray = camera.generateRay(pixel_coord, { uniform.gen(), uniform.gen() });
    glm::vec3 beta = {1, 1, 1};
    glm::vec3 color = {0, 0, 0};

    // For a simple ray tracing, we calculate the reflaction of every ray.
    // Depend on material, if it's specular, then using mirror reflaction
    // otherwise, we sample a random point on a half sphese.

    // To calculate the color of one pixel, every time when a ray hit a point
    // we need to add up (current light intensity * object color).
    // Albedo is basically the "color" of an object, means how many light will absorb by this object
    // Emissive is the light color of an object emit.
    // If an object is specular, it will behave like a mirror.

    // Using for or while?
    // I use for to prevent infinity loop
    for(int i=0; i<50; i++){
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
                } while (glm::length(light_direction) > 1);
                light_direction.y = glm::abs(light_direction.y);
                light_direction = glm::normalize(light_direction);
                light_direction = frame.worldFromLocal(light_direction);
            }

            ray.direction = light_direction;
            // ray.origin = hit_info->hit_point;
            ray.origin = hit_info->hit_point + light_direction * 1e-4f;
        }
        else{
            break;
        }
    }
    return color;
}