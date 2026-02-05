#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>

#include "chunk/chunk.hpp"
#include "../height-map/height-map.hpp"
#include "../utilities/math/math.hpp"
#include "../utilities/file-handling/text-file-handling/text-file-handling.hpp"
#include "../hitbox/hitbox.hpp"

#define WORLD_SAVE_FILE_DESCRIPTOR_NAME            ('0')
#define WORLD_SAVE_FILE_DESCRIPTOR_SIZE            ('1')
#define WORLD_SAVE_FILE_DESCRIPTOR_CHUNK_BEGIN     ('2')
#define WORLD_SAVE_FILE_DESCRIPTOR_CHUNK_END       ('3')
#define WORLD_SAVE_FILE_DESCRIPTOR_CHUNK_NEW_VOXEL ('4')
#define WORLD_SAVE_FILE_DESCRIPTOR_WORLD_SEED      ('5')

namespace SnazzCraft
{
    class World
    {
    public:
        std::string Name = "UNASSIGNED";
        unsigned int Size; // Size^2 = #Chunks
        int Seed;
        double HeightMapScale = 0.005; // Smaller = Smoother Terrain
        double HeightMapHeightMultiplier = 32.0;
        
        SnazzCraft::HeightMap* WorldHeightMap = nullptr;

        std::unordered_map<unsigned int, SnazzCraft::Chunk*>* Chunks = new std::unordered_map<unsigned int, SnazzCraft::Chunk*>();
        std::mutex ChunkMutex;

        World(std::string Name, unsigned int Size, int Seed);

        ~World();

        bool GenerateChunk(unsigned int X, unsigned int Z);

        void RenderChunks();

        void OptimizeChunks();

        bool IsCollidingVoxel(const SnazzCraft::Hitbox& Hitbox);

    private:


    };

    SnazzCraft::World* CreateWorld(std::string Name, unsigned int Size, int Seed);

    SnazzCraft::World* LoadWorldFromSaveFile(std::string FilePath);

    bool SaveWorldToFile(SnazzCraft::World* World, bool OverwriteExistingFile);

    extern SnazzCraft::World* CurrentWorld;
}
