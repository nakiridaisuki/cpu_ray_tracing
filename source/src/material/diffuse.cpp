#include "material/diffuse.hpp"
#include "sample/spherical.hpp"

glm::vec3 DiffuseMaterial::sampleBRDF(const glm::vec3 &view_direction, glm::vec3 &beta, Random &rng) const {
    beta *= albedo;
    return CosineSampleHemisphere({ rng.gen(), rng.gen() });
}