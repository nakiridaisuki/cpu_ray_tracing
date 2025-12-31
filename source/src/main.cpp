#include <iostream>
#include <glm/glm.hpp>
#include "thread_pool.hpp"
#include "film.hpp"
#include "timer.hpp"
#include "camera.hpp"
#include "sphere.hpp"

#include <stdio.h>

int main(){
    ThreadPool thread_pool;
    Timer timer;

    int width = 1920, height = 1080;
    int factor = 1;
    Film film(width/factor, height/factor);

    Camera camera {film, {0, 0, 5}, {0, 0, 0}, 40};
    Sphere sphere1 { {0, 0, 0}, 1.f };

    glm::vec3 light = {5, 5, 10};

    timer.start();        
    thread_pool.ParallelFor(film.getWidth(), film.getHeight(), [&](size_t x, size_t y){
        auto ray = camera.generateRay({x, y});
        auto result = sphere1.intersect(ray);
        if(result.has_value()){
            glm::vec3 hit_point = ray.hit(result.value());
            glm::vec3 normal = glm::normalize(hit_point - sphere1.center);
            glm::vec3 light_dir = glm::normalize(light - hit_point);
            float cosine = glm::max(0.f, glm::dot(normal, light_dir));

            film.setPixel(x, y, { cosine, cosine, cosine });
            // film.setPixel(x, y, { 0.2, 0.4, 0.6 });
        }
        else{
            film.setPixel(x, y, { 0, 0, 0 });
        }
    });
    thread_pool.wait();

    film.save("result.ppm");
    timer.end();


    std::cout << "Generate time: " << timer.ms() << " ms" << std::endl;

    return 0;
}