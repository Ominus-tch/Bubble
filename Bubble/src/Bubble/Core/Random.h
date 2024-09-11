#pragma once

#include <random>

namespace Bubble {

    class Random
    {
    public:
        static void Init();
        static float Float();
        static glm::vec3 Vec3();
        static glm::vec2 Vec2();
        static int Int(int min, int max);
        static int Next(float oldNumber);

    private:
        static std::mt19937 s_RandomEngine;
    };

}

