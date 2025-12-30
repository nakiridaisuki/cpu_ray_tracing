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

    Film file { 1920, 1080 };
    for(int i=0; i<1080; i++){
        for(int j=0; j<1920; j++){
            glm::vec3 color = {0.2, 0.4, 0.8};
            color *= ((i/50 + j/50) % 2 ? 0. : 1.);
            file.setPixel(i, j, color);
        }
    }
    file.save("test.ppm");
    std::cout << "Geneted" << std::endl;

    // ThreadPool thread_pool;
    // thread_pool.addTask(new SimpleTask());
    // thread_pool.addTask(new SimpleTask());
    // thread_pool.addTask(new SimpleTask());
    // thread_pool.addTask(new SimpleTask());
    // thread_pool.addTask(new SimpleTask());
    // thread_pool.addTask(new SimpleTask());
    // thread_pool.addTask(new SimpleTask());

    return 0;
}