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
#include "frame.hpp"
#include "rgb.hpp"
#include <random>

int main(){
    ThreadPool thread_pool;
    Timer timer;

    int width = 1920, height = 1080;
    int factor = 1;
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
        {2, 1, -1.5}
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

    std::atomic<int> count = 0;
    int spp = 128;

    std::mt19937 gen(2234124);
    std::uniform_real_distribution<float> uniform(-1, 1);

    timer.start();        
    thread_pool.ParallelFor(film.getWidth(), film.getHeight(), [&](size_t x, size_t y){

        for(int i=0; i<spp; i++){
            auto ray = camera.generateRay({x, y}, { abs(uniform(gen)), abs(uniform(gen)) });
            glm::vec3 beta = {1, 1, 1};
            glm::vec3 color = {0, 0, 0};
    
            while(true){
                auto hit_info = scene.intersect(ray);
                if(hit_info.has_value()){
                    color += beta * hit_info->material->emissive;
                    beta *= hit_info->material->albedo;
    
                    ray.origin = hit_info->hit_point;
    
                    Frame frame(hit_info->normal);
                    if(hit_info->material->is_specular){
                        glm::vec3 view_direction = frame.localFromWorld(-ray.direction);
                        view_direction.x = -view_direction.x;
                        view_direction.z = -view_direction.z;
                        ray.direction = frame.worldFromLocal(view_direction);
                    }
                    else{
                        do {
                            ray.direction = { uniform(gen), uniform(gen), uniform(gen) };
                        } while (glm::length(ray.direction) < 1);
                        ray.direction.y = glm::abs(ray.direction.y);
                        ray.direction = glm::normalize(ray.direction);
                        ray.direction = frame.worldFromLocal(ray.direction);
                    }
                }
                else{
                    break;
                }
            }
            film.addSample(x, y, color);
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