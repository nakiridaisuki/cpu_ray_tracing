#include "renderer/base.hpp"
#include "thread/thread_pool.hpp"
#include "utils/progress.hpp"
#include <algorithm>

void BaseRenderer::render(size_t spp, const std::filesystem::path &filename) {
    size_t current_spp = 0, increase = 1;

    Film &film = camera.getFilm();
    Progress progress(film.getWidth() * film.getHeight() * spp);

    while(current_spp < spp){
        // render 
        thread_pool.ParallelFor(film.getWidth(), film.getHeight(), [&](size_t x, size_t y){
            for(int i=0; i<increase; i++){
                film.addSample(x, y, renderPixel({x, y}));
            }
            progress.update(increase);
        });
        thread_pool.wait();

        film.save(filename);
        std::cout << "Save picture: " << filename.string() << ", current spp: " << current_spp << std::endl;

        current_spp += increase;
        increase = std::min<size_t>(increase*2, static_cast<size_t>(32));
        increase = std::min(increase, spp - current_spp);
    }
}