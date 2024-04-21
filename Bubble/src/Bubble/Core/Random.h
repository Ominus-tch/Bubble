#pragma once

#include <random>

namespace Bubble {

    class Random
    {
    public:
        static void Init();
        static float Float();
        static int Int(int min, int max);

    private:
        static std::mt19937 s_RandomEngine;
    };

}

