#include "material/specular.hpp"
#include "sample/spherical.hpp"

glm::vec3 SpecularMaterial::sampleBRDF(const glm::vec3 &view_direction, glm::vec3 &beta, Random &rng) const {
    beta *= albedo;
    return { -view_direction.x, view_direction.y, -view_direction.z };
}