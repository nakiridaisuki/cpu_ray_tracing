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
    Timer timer;

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
        { {1, 1, 1}, false, RGB(0xf0, 0x89, 0xb3) }, 
        {-1, 0.5, -3}
    );
    scene.addInstance(
        sphere,
        { {1, 1, 1}, false, RGB(0xb2, 0xf0, 0x89) }, 
        {-1, 0.5, 3}
    );
    scene.addInstance(
        sphere,
        { {1, 1, 1}, true}, 
        {2, 1, -2}
    );
    scene.addInstance(
        plane, 
        { {1, 1, 1}, false},
        {0, 0, 0}
    );
    scene.addInstance(
        model,
        { {1, 1, 1}, false, RGB(10, 10, 10) }, 
        {-1, .5f, 0}, 
        {2, 2, 2}
    );

    NormalRenderer normal_renderer(camera, scene);
    SimpleRTRenderer simple_raytracing(camera, scene);
    // timer.start();

    normal_renderer.render(1, "normal.ppm");

    camera.clearFilm();

    simple_raytracing.render(16, "RT.ppm");

    // normal_renderer.render(spp, "result.ppm");
    // timer.end();
    // std::cout << "Generate time: " << timer.ms() << " ms" << std::endl;



    return 0;
}