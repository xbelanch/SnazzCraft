#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <thread>
#include <mutex>

#include "chunk.hpp"
#include "../height-map/height-map.hpp"
#include "../utilities/math.hpp"
#include "../utilities/text-file-handling.hpp"
#include "../hitbox/hitbox.hpp"
#include "../entity/entity.hpp"
#include "../entity/user.hpp"
#include "../utilities/math.hpp"

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
        static constexpr unsigned int MaxSize = 2147483647; 

        std::string Name = "UNASSIGNED";
        unsigned int Size; // Size^2 = #Chunks
        int Seed;
        
        unsigned int RenderDistance = 50;

        World(std::string Name, unsigned int Size, int Seed);

        ~World();

        void GenerateChunk(unsigned int X, unsigned int Z);

        void RenderChunks(SnazzCraft::User* Player);

        SnazzCraft::Voxel* GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox) const; // Returns nullptr if no collision

        SnazzCraft::Voxel* GetCollidingVoxel(const glm::vec3& Position) const; // Returns nullptr if no collision

        void MoveEntity(SnazzCraft::Entity* Entity, const glm::vec3& Rotation, float Distance) const; // Returns true if movement occurred without voxel collision

        void MoveEntity(glm::vec3 Translation, SnazzCraft::Entity* Entity) const; // Returns true if movement occurred without voxel collision

        void UpdateAllLighting() const;

        bool SaveWorldToFile(bool OverwriteExistingFile);

        inline void ApplyGravityToEntities(std::vector<SnazzCraft::Entity*> AdditionalEntities)
        {
            constexpr float MoveDistance = 0.2f;

            for (auto& ChunkPair : *this->Chunks) {
                for (SnazzCraft::Entity* Entity : ChunkPair.second->Entities) {
                    this->MoveEntity(glm::vec3(0.0f, -MoveDistance, 0.0f), Entity);
                }
            }

            for (SnazzCraft::Entity* Entity : AdditionalEntities) {
                this->MoveEntity(glm::vec3(0.0f, -MoveDistance, 0.0f), Entity);
            }
        }

    private:
        std::unordered_map<unsigned int, SnazzCraft::Chunk*>* Chunks = nullptr;
        mutable std::mutex ChunksMutex;

        SnazzCraft::HeightMap* WorldHeightMap = nullptr;

        void ApplyLighting(int LightOrigin[3], int LightProducingLevel) const;

        void InitializeAndAddChunk(unsigned int X, unsigned int Z) const;
        
    public:
        static SnazzCraft::World* CreateWorld(std::string Name, unsigned int* Size, int Seed);

        static SnazzCraft::World* LoadWorldFromSaveFile(std::string FilePath);
    };

    
    extern SnazzCraft::World* CurrentWorld;
}
