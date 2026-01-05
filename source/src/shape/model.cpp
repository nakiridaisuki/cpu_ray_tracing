#include "shape/model.hpp"
#include <iostream>
#include <string.h>
#include <rapidobj/rapidobj.hpp>

#include "utils/profile.hpp"

Model::Model(const std::filesystem::path &filename) {
    PROFILE("Loading model " + filename.string())

    auto result = rapidobj::ParseFile(filename, rapidobj::MaterialLibrary::Ignore());
    auto &attri = result.attributes;

    for(auto &shape : result.shapes){
        auto &mesh = shape.mesh;
        
        int indices_offset = 0;
        for(auto num_vertices : mesh.num_face_vertices){
            if(num_vertices == 3){
                auto idx1 = mesh.indices[indices_offset];
                auto idx2 = mesh.indices[indices_offset + 1];
                auto idx3 = mesh.indices[indices_offset + 2];

                glm::vec3 pos1 = {
                    attri.positions[idx1.position_index * 3],
                    attri.positions[idx1.position_index * 3 + 1],
                    attri.positions[idx1.position_index * 3 + 2],
                };
                glm::vec3 pos2 = {
                    attri.positions[idx2.position_index * 3],
                    attri.positions[idx2.position_index * 3 + 1],
                    attri.positions[idx2.position_index * 3 + 2],
                };
                glm::vec3 pos3 = {
                    attri.positions[idx3.position_index * 3],
                    attri.positions[idx3.position_index * 3 + 1],
                    attri.positions[idx3.position_index * 3 + 2],
                };

                Triangle tri;
                if(idx1.normal_index >= 0 && idx2.normal_index >= 0 && idx3.normal_index >= 0){
                    glm::vec3 nor1 = {
                        attri.normals[idx1.normal_index * 3],
                        attri.normals[idx1.normal_index * 3 + 1],
                        attri.normals[idx1.normal_index * 3 + 2],
                    };
                    glm::vec3 nor2 = {
                        attri.normals[idx2.normal_index * 3],
                        attri.normals[idx2.normal_index * 3 + 1],
                        attri.normals[idx2.normal_index * 3 + 2],
                    };
                    glm::vec3 nor3 = {
                        attri.normals[idx3.normal_index * 3],
                        attri.normals[idx3.normal_index * 3 + 1],
                        attri.normals[idx3.normal_index * 3 + 2],
                    };

                    tri = Triangle(
                        pos1, pos2, pos3,
                        nor1, nor2, nor3
                    );
                }
                else{
                    tri = Triangle(pos1, pos2, pos3);
                    triangles.push_back(tri);
                }
                triangles.push_back(tri);
            }

            indices_offset += num_vertices;
        }
    }

    build();
}

std::optional<HitInfo> Model::intersect(const Ray &ray, float t_min, float t_max) const {
    if(!bound.intersect(ray, t_min, t_max)){
        return {};
    }

    std::optional<HitInfo> closest_hit_info {};
    for(const auto triangle : triangles){
        auto hit_info = triangle.intersect(ray, t_min, t_max);
        if(hit_info.has_value()) {
            t_max = hit_info->t;
            closest_hit_info = hit_info;
        }
    }
    return closest_hit_info;
}

void Model::build(){
    for(auto &tri:triangles){
        bound.extend(tri.p1);
        bound.extend(tri.p2);
        bound.extend(tri.p3);
    }
}