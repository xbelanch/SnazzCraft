#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <thread>
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>

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
        struct VoxelDDAResult
        {
            SnazzCraft::Voxel* CollidingVoxel = nullptr;
            glm::vec3 EndPosition = glm::vec3(0.0f);
        };

        std::string Name = "UNASSIGNED";
        unsigned int Size; // Size^2 = #Chunks
        int Seed;

        double HeightMapScale = 0.005; // Smaller = Smoother Terrain
        double HeightMapHeightMultiplier = 32.0;
        SnazzCraft::HeightMap* WorldHeightMap = nullptr;

        unsigned int RenderDistance = 50;

        std::unordered_map<unsigned int, SnazzCraft::Chunk*>* Chunks = new std::unordered_map<unsigned int, SnazzCraft::Chunk*>();
        std::mutex ChunkMutex;

        World(std::string Name, unsigned int Size, int Seed);

        ~World();

        bool GenerateChunk(unsigned int X, unsigned int Z);

        void RenderChunks(SnazzCraft::User* Player);

        void OptimizeChunks();

        SnazzCraft::Voxel* GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox); // Returns nullptr if no collision

        SnazzCraft::Voxel* GetCollidingVoxel(const glm::vec3& Position); // Returns nullptr if no collision

        void MoveEntity(SnazzCraft::Entity* Entity, const glm::vec3& Rotation, float Distance); // Returns true if movement occurred without voxel collision

        void MoveEntity(glm::vec3 Translation, SnazzCraft::Entity* Entity); // Returns true if movement occurred without voxel collision

        void UpdateLighting();

        void ApplyLighting(int LightOrigin[3], int LightProducingLevel);

        bool SaveWorldToFile(bool OverwriteExistingFile);

        bool PlaceVoxel(SnazzCraft::User* Player, unsigned int VoxelID);

        SnazzCraft::World::VoxelDDAResult* MarchDDAToVoxel(const glm::vec3& StartingPosition, const glm::vec3& FrontVector, float MaxDistance); // FrontVector should be normalized

        inline void ApplyGravityToEntities(std::vector<SnazzCraft::Entity*> AdditionalEntities)
        {
            const float MoveDistance = 0.2f;

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

        
    public:
        static SnazzCraft::World* CreateWorld(std::string Name, unsigned int Size, int Seed);

        static SnazzCraft::World* LoadWorldFromSaveFile(std::string FilePath);
    };

    
    extern SnazzCraft::World* CurrentWorld;
}
