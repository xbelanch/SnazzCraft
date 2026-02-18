#include "snazzcraft-engine/height-map/noise-module.hpp"

SnazzCraft::PerlinNoiseModule::PerlinNoiseModule(int Seed, double Frequency, double Persistence, double Lacunarity, int OctaveCount)
{
    this->Seed = Seed;
    this->Frequency = Frequency;
    this->Persistence = Persistence;
    this->Lacunarity = Lacunarity;
    this->OctaveCount = OctaveCount;

    this->NoiseModule.SetSeed(this->Seed);
    this->NoiseModule.SetFrequency(this->Frequency);
    this->NoiseModule.SetPersistence(this->Persistence);
    this->NoiseModule.SetLacunarity(this->Lacunarity);
    this->NoiseModule.SetOctaveCount(this->OctaveCount);
}

SnazzCraft::PerlinNoiseModule::~PerlinNoiseModule()
{
    
}
