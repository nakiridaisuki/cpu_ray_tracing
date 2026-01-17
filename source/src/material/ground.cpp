#include "material/ground.hpp"
#include "sample/spherical.hpp"

glm::vec3 GroundMaterial::sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, glm::vec3 &beta, Random &rng) const {
    glm::vec3 dis = glm::abs(hit_point - glm::round(hit_point));
    if(dis.x < 0.1 || dis.z < 0.1){
        beta *= 0.1;
    }
    else{
        beta *= albedo;
    }
    return CosineSampleHemisphere({ rng.gen(), rng.gen() });
}