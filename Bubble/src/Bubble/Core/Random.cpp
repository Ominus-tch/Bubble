#include "bgpch.h"
#include "Random.h"

namespace Bubble {

    std::mt19937 Random::s_RandomEngine;

    void Random::Init()
    {
        s_RandomEngine.seed(std::random_device()());
    }

    float Random::Float()
    {
        return std::generate_canonical<float, std::numeric_limits<float>::digits>(s_RandomEngine);
    }

    int Random::Int(int min, int max)
    {
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(s_RandomEngine);
    }

}