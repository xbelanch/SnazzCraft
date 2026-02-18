#pragma once

#include "../../noise/noise.h"

namespace SnazzCraft
{
    class PerlinNoiseModule
    {
    public:
        noise::module::Perlin NoiseModule;

        int Seed;
        double Frequency;
        double Persistence;
        double Lacunarity;
        int OctaveCount;

        PerlinNoiseModule(int Seed, double Frequency, double Persistence, double Lacunarity, int OctaveCount);

        ~PerlinNoiseModule();



    private:


    };


}
