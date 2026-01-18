#include "renderer/base.hpp"
#include "thread/thread_pool.hpp"
#include "utils/progress.hpp"
#include <algorithm>

#include "utils/profile.hpp"

void BaseRenderer::render(size_t spp, const std::filesystem::path &filename) {
    PROFILE("Rendering image " + filename.string() + " with " + std::to_string(spp) + " spp")

    size_t current_spp = 0, increase = 1;

    Film &film = camera.getFilm();
    film.clear();
    Progress progress(film.getWidth() * film.getHeight() * spp, 20);

    while(current_spp < spp){

        PROFILE("Rendering image " + std::to_string(increase) + " spp " + filename.string())
        // Parallel render a pixel
        // Render `increase` sample point every time for dinamic update picture
        thread_pool.ParallelFor(film.getWidth(), film.getHeight(), [&](size_t x, size_t y){
            for(int i=0; i<increase; i++){
                film.addSample(x, y, renderPixel({x, y, current_spp + i}));
            }
            progress.update(increase);
        });
        thread_pool.wait();

        current_spp += increase;
        increase = std::min<size_t>(increase*2, static_cast<size_t>(32));
        increase = std::min(increase, spp - current_spp);
        
        film.save(filename);
        std::cout << "Save picture: " << filename.string() << ", current spp: " << current_spp << std::endl;
    }
}