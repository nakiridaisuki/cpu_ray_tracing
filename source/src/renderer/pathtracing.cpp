#include "renderer/pathtracing.hpp"
#include "utils/frame.hpp"
#include "utils/random.hpp"

glm::vec3 PathTracingRenderer::renderPixel(const glm::ivec2 &pixel_coord) {
    auto ray = camera.generateRay(pixel_coord, { uniform.gen(), uniform.gen() });
    glm::vec3 beta = {1, 1, 1};
    glm::vec3 L = {0, 0, 0};
    float q = 0.9;

    while(true){
        auto hit_info = scene.intersect(ray);
        if(hit_info.has_value()){
            L += (beta * hit_info->material->emissive);

            if(uniform.gen() > q){
                break;
            }
            beta /= q;

            Frame frame(hit_info->normal);
            glm::vec3 light_direction;
            
            if(hit_info->material){
                glm::vec3 view_direction = frame.localFromWorld(-ray.direction);
                light_direction = hit_info->material->sampleBRDF(view_direction, beta, uniform);
            }
            else{
                break;
            }
            
            ray.origin = hit_info->hit_point + light_direction * std::numeric_limits<float>::epsilon();
            ray.direction = frame.worldFromLocal(light_direction);
        }
        else{
            break;
        }
    }
    
    return L;
}