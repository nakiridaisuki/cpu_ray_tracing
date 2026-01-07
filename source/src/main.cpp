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
#include "renderer/simple_rt.hpp"
#include "renderer/debug.hpp"

#include "thread/thread_pool.hpp"

int main(){

    std::ios::sync_with_stdio(0);
    std::cout.tie(0);

    int width = 1920, height = 1080;
    int factor = 2;
    Film film(width/factor, height/factor);

    Camera camera {film, {5, .5, 0}, {0, 0, 0}, 40};
    
    Sphere sphere { {0, 0, 0}, 1 };
    Model model("model_and_uv/dragon_871k.obj");
    // Model model("model_and_uv/simple_dragon.obj");
    Plane plane( {0, 0, 0}, {0, 1, 0} );
    
    Scene scene {};
    // scene.addInstance(
    //     sphere,
    //     Material( {1, 1, 1}, false, RGB(0xf0, 0x89, 0xb3) ), 
    //     {-1, 0.5, -3}
    // );
    // scene.addInstance(
    //     sphere,
    //     Material( {1, 1, 1}, false, RGB(0xb2, 0xf0, 0x89) ), 
    //     {-1, 0.5, 3}
    // );
    // scene.addInstance(
    //     sphere,
    //     { {1, 1, 1}, true }, 
    //     {2, 1, -2}
    // );
    // scene.addInstance(
    //     plane, 
    //     { {1, 1, 1}, false },
    //     {0, 0, 0}
    // );
    scene.addInstance(
        model,
        { {1, 1, 1}, false, RGB(10, 10, 10) }, 
        {0, 0, 0}, 
        {3, 3, 3}
    );

    // scene.addInstance(
    //     sphere,
    //     { {1, 1, 1}, false, RGB(0xff, 0xff, 0xff) },
    //     {3, 0, 0}
    // );
    // scene.addInstance(
    //     plane,
    //     { {0, 1, 1}, false },
    //     {0, 0, 2 },
    //     { 1, 1, 1 },
    //     { -90, 0, 0 }
    // );
    // scene.addInstance(
    //     plane,
    //     { {1, 0, 1}, false },
    //     {0, 0, -2 },
    //     { 1, 1, 1 },
    //     { 90, 0, 0 }
    // );

    SimpleRTRenderer simple_raytracing(camera, scene);
    NormalRenderer normal_renderer(camera, scene);
    
    // normal_renderer.render(1, "normal.ppm");

    
    // simple_raytracing.render(128, "RT.ppm");
    
    BVHDepthRenderer bvh_depth_renderer(camera, scene);
    TriangleTestCountRenderer tri_test_renderer(camera, scene);
    BoundTestCountRenderer bnd_test_renderer(camera, scene);
    
    bvh_depth_renderer.render(4, "bvh_depth.ppm");
    tri_test_renderer.render(4, "tri_test.ppm");
    bnd_test_renderer.render(4, "bnd_test.ppm");
    
    
    // for(int i=0; i<120; i++){
    //     float pos = -5.f + (4.f * i / 120.f);
    //     camera = {film, {pos, 0, 0}, {0, 0, 0}, 40};
    //     NormalRenderer normal_renderer(camera, scene);

    //     char buffer[256];
    //     snprintf(buffer, sizeof(buffer), "result/result_%02d.ppm", i);
    //     normal_renderer.render(1, buffer);
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