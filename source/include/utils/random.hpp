#pragma once

#include <random>

class Random {
public:
    Random(const unsigned int seed) {
        generator.seed(seed);
        std::uniform_real_distribution<float>::param_type range(0, 1);
        uniform_distribution.param(range);
    }
    Random() { Random(0); }
    
    void setSeed(const unsigned int seed) { generator.seed(seed); }
    void setRange(const float from, const float to){
        std::uniform_real_distribution<float>::param_type range(from, to);
        uniform_distribution.param(range);
    }

    float gen() { return uniform_distribution(generator); }
    float gen(float from, float to) { return gen() * (to - from) + from; }

private:
    std::mt19937 generator;
    std::uniform_real_distribution<float> uniform_distribution;
};