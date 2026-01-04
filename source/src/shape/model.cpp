#include "shape/model.hpp"
#include <iostream>
#include <string.h>

#include "utils/profile.hpp"

Model::Model(const std::filesystem::path &filename) {
    PROFILE("Loading model " + filename.string())

    // vertices : v xx yy zz
    std::vector<glm::vec3> positions;
    // normals : vn xx yy zz
    std::vector<glm::vec3> normals;
    
    // triangles : f p1//n1 p2//n2 p3//n3 (index of position // index of normal)
    // .obj's index start from 1
    positions.push_back({});
    normals.push_back({});
    
    FILE *cfile = fopen(filename.string().c_str(), "r");
    if(cfile == nullptr){
        std::cout << "Fail to open file: " << filename.string() << std::endl;
        return;
    }

    std::cout << "Start import model..." << std::endl;
    char type[256];
    while(fscanf(cfile, "%s", type) != EOF){
        if(strcmp(type, "v") == 0){
            glm::vec3 pos;
            fscanf(cfile,"%f %f %f", &pos.x, &pos.y, &pos.z);
            positions.push_back(pos);
        }
        else if(strcmp(type, "vn") == 0){
            glm::vec3 nor;
            fscanf(cfile,"%f %f %f", &nor.x, &nor.y, &nor.z);
            normals.push_back(nor);
        }
        else if(strcmp(type, "f") == 0){
            int p1, p2, p3;
            int n1, n2, n3;
            fscanf(cfile,"%d//%d %d//%d %d//%d", &p1, &n1, &p2, &n2, &p3, &n3);
            Triangle tri = {
                positions[p1], positions[p2], positions[p3],
                normals[n1], normals[n2], normals[n3],
            };
            triangles.push_back(tri);
        }
        else{
            char trash[1024];
            fgets(trash, sizeof(trash), cfile);
        }
    }
    std::cout << "Model import complete, has " << triangles.size() << " triangles." << std::endl;
    fclose(cfile);
}

std::optional<HitInfo> Model::intersect(const Ray &ray, float t_min, float t_max) const {
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