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

    int Random::Next(float oldNumber)
    {
        // Convert the float seed to an integer using std::hash
        std::hash<float> hasher;
        size_t seed = hasher(oldNumber);

        // Reseed the s_RandomEngine with the hash value
        s_RandomEngine.seed(static_cast<std::mt19937::result_type>(seed));

        // Generate a random integer using the reseeded engine
        std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        return distribution(s_RandomEngine);
    }

}