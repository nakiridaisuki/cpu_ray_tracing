#include "material/dielectric.hpp"
#include "sample/spherical.hpp"
#include <iostream>

// reflaction rate by Fersnel
float Fersnel_Fr(const float &etai_divi_etat, const float &cos_i, float &cos_t) {
    float sin2_i = 1 - cos_i * cos_i;
    float sin2_t = sin2_i * etai_divi_etat * etai_divi_etat;

    if(sin2_t > 1){
        return 1;
    }

    cos_t = glm::sqrt(1 - sin2_t);
    float r_paral = (cos_i - etai_divi_etat * cos_t) / (cos_i + etai_divi_etat * cos_t);
    float r_perpe = (etai_divi_etat * cos_i - cos_t) / (etai_divi_etat * cos_i + cos_t);
    return 0.5 * (r_paral * r_paral + r_perpe * r_perpe);
}

glm::vec3 DielectricMaterial::sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, glm::vec3 &beta, Random &rng) const {
    float etai_divi_etat = 1.f / ior;
    float cos_i = view_direction.y, cos_t = 0;
    glm::vec3 normal = {0, 1, 0};
    if(cos_i < 0){
        cos_i = -cos_i;
        etai_divi_etat = ior;
        normal.y = -normal.y;
    }
    
    float fr = Fersnel_Fr(etai_divi_etat, cos_i, cos_t);
    
    beta *= albedo_t;
    if(rng.gen() < fr){
        beta *= albedo_r;
        return { -view_direction.x, view_direction.y, -view_direction.z };
    }
    return -etai_divi_etat * view_direction + (etai_divi_etat * cos_i - cos_t) * normal;
}