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

int main(){

    int width = 1920, height = 1080;
    int factor = 5;
    Film film(width/factor, height/factor);

    Camera camera {film, {-5, .5, 0}, {0, .2, 0}, 40};
    
    Sphere sphere { {0, 0, 0}, 1 };
    Model model("model_and_uv/simple_dragon.obj");
    Plane plane( {0, 0, 0}, {0, 1, 0} );
    
    Scene scene {};
    scene.addInstance(
        sphere,
        Material( {1, 1, 1}, false, RGB(0xf0, 0x89, 0xb3) ), 
        {-1, 0.5, -3}
    );
    scene.addInstance(
        sphere,
        Material( {1, 1, 1}, false, RGB(0xb2, 0xf0, 0x89) ), 
        {-1, 0.5, 3}
    );
    scene.addInstance(
        sphere,
        { {1, 1, 1}, true }, 
        {2, 1, -2}
    );
    scene.addInstance(
        plane, 
        { {1, 1, 1}, false },
        {0, 0, 0}
    );
    scene.addInstance(
        model,
        { {1, 1, 1}, false, RGB(10, 10, 10) }, 
        {-1, .5f, 0}, 
        {2, 2, 2}
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

    NormalRenderer normal_renderer(camera, scene);
    SimpleRTRenderer simple_raytracing(camera, scene);
    
    // normal_renderer.render(1, "normal.ppm");

    simple_raytracing.render(128, "RT.ppm");



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