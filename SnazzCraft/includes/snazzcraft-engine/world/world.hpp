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
#include <stdint.h>

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
        static constexpr uint32_t MaxSize = 2147483647u; 

        std::string Name = "UNASSIGNED";
        uint32_t Size; // Size^2 = #Chunks
        int32_t Seed;
        
        uint32_t RenderDistance = 50;

        World(std::string Name, uint32_t Size, int32_t Seed);

        ~World();

        void GenerateChunk(uint32_t X, uint32_t Z);

        void RenderChunks(SnazzCraft::User* Player);

        SnazzCraft::Voxel* GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox) const; // Returns nullptr if no collision & not thread safe

        SnazzCraft::Voxel* GetCollidingVoxel(const glm::vec3& Position) const; // Returns nullptr if no collision & not thread safe

        void MoveEntity(SnazzCraft::Entity* Entity, const glm::vec3& Rotation, float Distance) const; // Returns true if movement occurred without voxel collision

        void MoveEntity(glm::vec3 Translation, SnazzCraft::Entity* Entity) const; // Returns true if movement occurred without voxel collision

        void UpdateAndApplyAllLighting() const;

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
        std::vector<SnazzCraft::Chunk*>* ChunkMeshUpdateQueue = nullptr;
        mutable std::mutex ChunkMeshUpdateQueueMutex;

        std::unordered_map<uint32_t, SnazzCraft::Chunk*>* Chunks = nullptr;
        mutable std::mutex ChunkMutex;

        SnazzCraft::HeightMap* WorldHeightMap = nullptr;
        mutable std::mutex HeightMapMutex;

        std::unordered_set<uint32_t>* ApplyLightingQueue = nullptr;
        mutable std::mutex ApplyLightingQueueMutex;

        void ApplyLightingVoxel(int32_t LightOrigin[3], int32_t LightProducingLevel, bool AutoUpdateChunks) const;

        void InitializeAndAddChunk(uint32_t X, uint32_t Z) const;

        inline void ApplyLightingChunk(SnazzCraft::Chunk* Chunk) const
        {
            for (auto& VoxelPair : *Chunk->Voxels) {
                if (VoxelPair.second.LightProducingLevel <= 0) continue;

                int32_t Position[3] = {
                    static_cast<int32_t>(VoxelPair.second.Position[0]) + Chunk->Position[0] * SnazzCraft::Chunk::Width,
                    static_cast<int32_t>(VoxelPair.second.Position[1]),
                    static_cast<int32_t>(VoxelPair.second.Position[2]) + Chunk->Position[1] * SnazzCraft::Chunk::Depth,
                };
                this->ApplyLightingVoxel(Position, VoxelPair.second.LightProducingLevel, false);
            }
        }

        inline void UpdateChunkMeshesInQueue()
        {
            std::lock_guard<std::mutex> QueueLockGuard(this->ChunkMeshUpdateQueueMutex);
            std::lock_guard<std::mutex> ChunkLockGuard(this->ChunkMutex);
            for (SnazzCraft::Chunk* Chunk : *this->ChunkMeshUpdateQueue) {
                Chunk->UpdateMesh();
            }
            this->ChunkMeshUpdateQueue->clear();
        }

        inline void UpdateLightingInQueue()
        {
            
        }
        
    public:
        static SnazzCraft::World* CreateWorld(std::string Name, uint32_t Size, int32_t Seed);

        static SnazzCraft::World* LoadWorldFromSaveFile(std::string FilePath);
    };
    
    extern SnazzCraft::World* CurrentWorld;
}
