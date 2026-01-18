#include "material/conductor.hpp"
#include "sample/spherical.hpp"
#include "utils/complex.hpp"

glm::vec3 ConductorMaterial::sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, glm::vec3 &beta, Random &rng) const {
    glm::vec3 fr;

    for(size_t i=0; i<3; i++){
        Complex etat_divi_etai = { ior[i], k[i] };
        float cos_i = glm::clamp(view_direction.y, 0.f, 1.f);
        float sin2_i = 1.f - cos_i * cos_i;
        Complex sin2_t = sin2_i / (etat_divi_etai * etat_divi_etai);
        Complex cos_t = sqrt(1.f - sin2_t);

        Complex r_paral = (cos_i * etat_divi_etai - cos_t) / (cos_i * etat_divi_etai + cos_t);
        Complex r_perpe = (cos_i - cos_t * etat_divi_etai) / (cos_i + cos_t * etat_divi_etai);

        float nr_paral = norm(r_paral);
        float nr_perpe = norm(r_perpe);
        fr[i] = (0.5 * (nr_paral*nr_paral + nr_perpe*nr_perpe));
    }
    
    beta *= fr;
    return { -view_direction.x, view_direction.y, -view_direction.z };
}