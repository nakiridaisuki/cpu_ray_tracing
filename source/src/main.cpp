#include <iostream>
#include <glm/glm.hpp>
#include "thread_pool.hpp"
#include "film.hpp"

class SimpleTask : public Task {
public:
    void run() override {
        std::cout << "Hello world" << std::endl;
    }
};

int main(){

    ThreadPool thread_pool {};

    Film file { 1920, 1080 };
    // auto now = std::chrono::high_resolution_clock::now();
    // for(int x=0; x<1920; x++){
    //     for(int y=0; y<1080; y++){
    //         // glm::vec3 color = {0.2, 0.4, 0.8};
    //         // glm::vec3 color = {x/1920., 0, 0};
    //         glm::vec3 color = {x/1920., y/1080., 0};
    //         // color *= ((y/50 + x/50) % 2 ? 0. : 1.);
    //         file.setPixel(x, y, color);
    //     }
    // }
    // auto time = std::chrono::high_resolution_clock::now() - now;
    // auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
    // std::cout << "Output Time: " << ms.count() << " ms" << std::endl;
            
    auto now = std::chrono::high_resolution_clock::now();
    size_t width = 1920, height = 1080;
    size_t factor = 120;
    thread_pool.paralleFor(width/factor, height/factor, [&](size_t x, size_t y){
        for(int i=x*factor; i<(x+1)*factor; i++){
            for(int j=y*factor; j<(y+1)*factor; j++){
                glm::vec3 color = {i/float(width*factor), j/float(height*factor), 0};
                file.setPixel(i, j, color);
            }
        }
    });
    auto time = std::chrono::high_resolution_clock::now() - now;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
    std::cout << "Output Time: " << ms.count() << " ms" << std::endl;

    // thread_pool.paralleFor(10, 5, [&](size_t x, size_t y){
    //     // glm::vec3 color = {0.2, 0.4, 0.8};
    //     // color *= ((x/50 + y/50) % 2 ? 0. : 1.);
    //     // file.setPixel(x, y, color);
    //     std::cout << x << " " << y << std::endl;
    // });

    thread_pool.wait();
    now = std::chrono::high_resolution_clock::now();
    file.save("test_p6.ppm");
    time = std::chrono::high_resolution_clock::now() - now;
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
    std::cout << "Save Time: " << ms.count() << " ms" << std::endl;

    return 0;
}