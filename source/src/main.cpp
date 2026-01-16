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

int main(){

    std::ios::sync_with_stdio(0);
    std::cout.tie(0);

    int width = 1920, height = 1080;
    int factor = 2;
    Film film(width/factor, height/factor);

    Camera camera { film, { -12, 5, -12 }, { 0, 0, 0 }, 45 };
    // Camera camera { film, { -3.6, 0, 0 }, { 0, 0, 0 }, 45 };
    
    Sphere sphere { {0, 0, 0}, 1 };
    Model model("model_and_uv/dragon_871k.obj");
    // Model model("model_and_uv/simple_dragon.obj");
    Plane plane( {0, 0, 0}, {0, 1, 0} );
    
    Scene scene {};

    Random rng(388);
    for (int i = 0; i < 5000; i ++) {
        glm::vec3 random_pos {
            rng.gen() * 100 - 50,
            rng.gen() * 2,
            rng.gen() * 100 - 50,
        };
        float u = rng.gen();
        if (u < 0.9) {
            BaseMaterial *material;
            if(rng.gen() > 0.5){
                material = new DiffuseMaterial( RGB(202, 159, 117) );
            }
            else{
                material = new SpecularMaterial( RGB(202, 159, 117) );
            }

            scene.addInstance(
                model,
                material,
                random_pos,
                { 1, 1, 1 },
                { rng.gen() * 360, rng.gen() * 360, rng.gen() * 360 }
            );
        } else if (u < 0.95) {
            auto *material = new SpecularMaterial({ rng.gen(), rng.gen(), rng.gen() });
            scene.addInstance(
                sphere,
                material,
                random_pos,
                { 0.4, 0.4, 0.4 }
            );
        } else {
            auto *material = new DiffuseMaterial({ 1, 1, 1 });
            material->setEmissive({ rng.gen() * 4, rng.gen() * 4, rng.gen() * 4 });
            random_pos.y += 6;
            scene.addInstance(
                sphere,
                material,
                random_pos
            );
        }
    }
    scene.addInstance(plane, new DiffuseMaterial(RGB(120, 204, 157)), { 0, -0.5, 0 });
    
    // scene.addInstance(
    //     model,
    //     { RGB(202, 159, 117) },
    //     { 0, 0, 0 },
    //     { 1, 3, 2 }
    // );
    // scene.addInstance(
    //     sphere,
    //     { { 1, 1, 1 }, false, RGB(255, 128, 128) },
    //     { 0, 0, 2.5 }
    // );
    // scene.addInstance(
    //     sphere,
    //     { { 1, 1, 1 }, false, RGB(128, 128, 255) },
    //     { 0, 0, -2.5 }
    // );
    // scene.addInstance(
    //     sphere,
    //     { { 1, 1, 1 }, true },
    //     { 3, 0.5, -2 }
    // );
    // scene.addInstance(plane, { RGB(120, 204, 157) }, { 0, -0.5, 0 });
    
    scene.build();

    NormalRenderer normal_renderer(camera, scene);
    PathTracingRenderer pt_renderer(camera, scene);
    
    // normal_renderer.render(1, "normal.ppm");

    
    // simple_raytracing.render(128, "RT.ppm");
    pt_renderer.render(128, "PT.ppm");
    
    // TriangleTestCountRenderer tri_test_renderer(camera, scene);
    // BoundTestCountRenderer bnd_test_renderer(camera, scene);
    
    // tri_test_renderer.render(1, "debug_tri_test.ppm");
    // bnd_test_renderer.render(1, "debug_bnd_test.ppm");
    
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