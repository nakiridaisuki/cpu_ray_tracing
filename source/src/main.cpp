#include <iostream>
#include <glm/glm.hpp>
#include "thread_pool.hpp"
#include "film.hpp"
#include "timer.hpp"
#include "camera.hpp"
#include "sphere.hpp"
#include "shape.hpp"
#include "model.hpp"
#include "plane.hpp"
#include "scene.hpp"

int main(){
    ThreadPool thread_pool;
    Timer timer;

    int width = 1920, height = 1080;
    int factor = 2;
    Film film(width/factor, height/factor);

    Camera camera {film, {-5, .2, 0}, {0, 0, 0}, 40};
    
    Sphere sphere { {0, 0, 0}, .5f };
    Triangle triangle { 
        {0, 0, -1}, 
        {0, 1, 1}, 
        {0, -1, 1}, 
    };
    Model model("model_and_uv/simple_dragon.obj");
    Plane plane( {0, 0, 0}, {0, 1, 0} );
    
    
    Scene scene {};
    scene.addInstance(sphere, {0, 0, -1});
    scene.addInstance(plane, {0, 0, 0});
    scene.addInstance(model, {1, 1, 0}, {1, 2, 3}, {30, 0, 0});

    glm::vec3 light = {-2, 5, 2};

    std::atomic<int> count = 0;

    timer.start();        
    thread_pool.ParallelFor(film.getWidth(), film.getHeight(), [&](size_t x, size_t y){
        auto ray = camera.generateRay({x, y});
        auto hit_info = scene.intersect(ray);
        if(hit_info.has_value()){
            glm::vec3 light_dir = glm::normalize(light - hit_info->hit_point);
            float cosine = glm::max(0.f, glm::dot(hit_info->normal, light_dir));

            film.setPixel(x, y, { cosine, cosine, cosine });
            // film.setPixel(x, y, { 1, 1, 1 });
            // film.setPixel(x, y, { 0.2, 0.4, 0.6 });
        }
        else{
            film.setPixel(x, y, { 0, 0, 0 });
        }

        count++;

        if(count % film.getWidth() == 0){
            std::cout << static_cast<float>(count) / (film.getWidth() * film.getHeight()) << std::endl;
        }
    });
    thread_pool.wait();

    film.save("result.ppm");
    timer.end();


    std::cout << "Generate time: " << timer.ms() << " ms" << std::endl;

    return 0;
}