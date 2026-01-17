#include <iostream>
#include <glm/glm.hpp>
#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "shape/sphere.hpp"
#include "shape/shape.hpp"
#include "shape/model.hpp"
#include "shape/plane.hpp"
#include "shape/scene.hpp"
#include "utils/timer.hpp"
#include "utils/rgb.hpp"
#include "renderer/normal.hpp"
#include "renderer/debug.hpp"
#include "renderer/pathtracing.hpp"
#include "material/diffuse.hpp"
#include "material/specular.hpp"
#include "material/dielectric.hpp"
#include "material/ground.hpp"

#include "utils/frame.hpp"

int main(){

    std::ios::sync_with_stdio(0);
    std::cout.tie(0);

    int width = 1920, height = 1080;
    int factor = 2;
    Film film(width/factor, height/factor);

    float x_pos = 0;
    // Camera camera { film, { x_pos, 15, 0.1 }, { x_pos, 0, 0 }, 40 };
    Camera camera { film, { x_pos, 2, 15 }, { x_pos, 0, 0 }, 40 };
    
    Sphere sphere { {0, 0, 0}, 1 };
    Model model("model_and_uv/dragon_871k.obj");
    Plane plane( {0, 0, 0}, {0, 1, 0} );
    
    Scene scene {};

    Random rng(388);
    for(int i=-3; i<=3; i++){
        float ior = (i+3) / 6.f * 1.5f + 1.f;
        scene.addInstance(
            sphere,
            new DielectricMaterial(
                ior,
                {1, 1, 1}
            ),
            {i*2, 0.5, 0},
            glm::vec3(0.8)
        );
    }
    scene.addInstance(plane, new GroundMaterial(RGB(120, 204, 157)), { 0, -1, 0 });
    
    auto *light_material = new DiffuseMaterial({1, 1, 1});
    light_material->setEmissive({0.95, 0.95, 1});
    scene.addInstance(plane, light_material, {0, 20, 0});
    
    scene.build();

    PathTracingRenderer pt_renderer(camera, scene);
    pt_renderer.render(128, "PT.ppm");
    
    NormalRenderer normal_renderer(camera, scene);
    normal_renderer.render(1, "normal.ppm");

    
    TriangleTestCountRenderer tri_test_renderer(camera, scene);
    tri_test_renderer.render(1, "debug_tri_test.ppm");
    
    BoundTestCountRenderer bnd_test_renderer(camera, scene);
    bnd_test_renderer.render(1, "debug_bnd_test.ppm");
    
    // int total = 480;
    // for(int i=0; i<total; i++){
    //     float theta = 6.28 * i / total;
    //     camera = {film, { -30.f * glm::cos(theta), 12, -30.f * glm::sin(theta)}, {0, 0, 0}, 40};
    //     BoundTestCountRenderer bnd_test_renderer(camera, scene);
    //     // NormalRenderer normal_renderer(camera, scene);

    //     char buffer[256];
    //     snprintf(buffer, sizeof(buffer), "result/result_%03d.ppm", i);
    //     bnd_test_renderer.render(1, buffer);
    // }

    return 0;
}

// Debug record

// Original
// ------- 640x360,  16spp,  1724 triangles ----------
// Loading model 52 ms
// Parallel For: 600 ms
// Save image: 7 ms
// Render 1spp: 12048 ms
// Render 16spp: 192349 ms

// Build release 
// ------- 640x360,  16spp,  1724 triangles ----------
// Loading model 52 ms
// Parallel For: 130 ms
// Save image: 2 ms
// Render 1spp: 655 ms
// Render 16spp: 7711 ms
// ------- 640x360,  128spp,  1724 triangles ----------
// Loading model 52 ms
// Parallel For: 120 ms
// Save image: 2 ms
// Render 1spp: 672 ms
// Render 16spp: 53365 ms

// Change ParallelFor
// ------- 640x360,  128spp,  1724 triangles ----------
// Parallel For: 0 ms

// Add AABB
// ------- 640x360,  128spp,  1724 triangles ----------
// Loading model 52 ms
// Render 1spp: 134 ms
// Render 16spp: 17531 ms

// Using rapidobj
// ------- 640x360,  128spp,  1724 triangles ----------
// Loading model 2 ms

// Add BVH
// ------- 640x360,  128spp,  1724 triangles ----------
// Loading model 16 ms
// Render 1spp: 13 ms
// Render 16spp: 1228 ms
// ------- 640x360,  128spp,  87k triangles ----------
// Loading model 939 ms
// Render 1spp: 85 ms
// Render 16spp: 9389 ms

// Change loop BVH intersect check
// ------- 640x360,  128spp,  87k triangles ----------
// Loading model 955 ms
// Render 1spp: 73 ms
// Render 16spp: 7013 ms

// Memory align
// ------- 640x360,  128spp,  87k triangles ----------
// Loading model 746 ms
// Render 1spp: 79 ms
// Render 16spp: 6816 ms

// BVH split axis checking
// ------- 640x360,  128spp,  87k triangles ----------
// Loading model 746 ms
// Render 1spp: 54 ms
// Render 16spp: 7107 ms

// Using inv_direc for bound interection checking
// ------- 640x360,  128spp,  87k triangles ----------
// Loading model 746 ms
// Render 1spp: 45 ms
// Render 16spp: 7107 ms

// ################### BVH Optimize ##################
// Original
// BVH state: total nodes 142643
// BVH state: leaf nodes 71322
// BVH state: triangles 87130
// BVH state: Mean leaf node triangle 1.22164
// BVH state: Maximum leaf node triangle cnt 23
// Profile Loading model model_and_uv/dragon_87k.obj cost 932 ms

// With SAH
// BVH state: total nodes 173829
// BVH state: leaf nodes 86915
// BVH state: triangles 87130
// BVH state: Mean leaf node triangle 1.00247
// BVH state: Maximum leaf node triangle cnt 4
// Profile Loading model model_and_uv/dragon_87k.obj cost 1068 ms

// BVH state: total nodes 1742425
// BVH state: leaf nodes 871213
// BVH state: triangles 871306
// BVH state: Mean leaf node triangle 1.00011
// BVH state: Maximum leaf node triangle cnt 3
// Profile Loading model model_and_uv/dragon_871k.obj cost 18709 ms

// Using triangles indices
// Profile Loading model model_and_uv/dragon_871k.obj cost 16181 ms

// Using bucket
// Profile Loading model model_and_uv/dragon_871k.obj cost 12181 ms

// Using Inplace partition
// Profile Loading model model_and_uv/dragon_871k.obj cost 2215 ms

// Using pre-calculate prefix and postfix
// Profile Loading model model_and_uv/dragon_871k.obj cost 2215 ms
// ################### BVH Optimize ##################

// Rendering 50 instance in scene
// Profile Rendering image RT.ppm with 128 spp cost 30183 ms

// With scene BVH
// Profile Rendering image RT.ppm with 128 spp cost 14507 ms