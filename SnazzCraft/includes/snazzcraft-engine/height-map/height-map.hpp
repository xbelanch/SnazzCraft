#pragma once

#include <unordered_map>

#include "noise-module.hpp"
#include "../utilities/math.hpp"

namespace SnazzCraft
{
    struct HeightMap
    {
    public:
        std::unordered_map<unsigned int, unsigned>* HeightValues = new std::unordered_map<unsigned int, unsigned>();
        SnazzCraft::PerlinNoiseModule* NoiseModule = nullptr;
        

        HeightMap(unsigned int Size, int HeightConstraintLow, int HeightConstraintHigh, int Seed, double Frequency, double Persistence, double Lacunarity, int OctaveCount);
        ~HeightMap();

        void GenerateValue(unsigned int X, unsigned int Y);

    private:
        unsigned int Size;
        int HeightConstraintLow;
        int HeightConstraintHigh;

    };
}
