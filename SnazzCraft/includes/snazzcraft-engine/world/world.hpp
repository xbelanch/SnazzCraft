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
#include <queue>

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

        std::unordered_map<uint32_t, SnazzCraft::Chunk*> Chunks;

        World(std::string IName, uint32_t ISize, int32_t ISeed);

        ~World();

        void GenerateChunk(uint32_t X, uint32_t Z, bool ApplyLighting); // Thread safe

        void RenderChunks(SnazzCraft::User* Player); // Thread safe

        SnazzCraft::Voxel* GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox) const; // Returns nullptr if no collision - not thread safe

        SnazzCraft::Voxel* GetCollidingVoxel(const glm::vec3& Position) const; // Returns nullptr if no collision - not thread safe

        void MoveEntity(SnazzCraft::Entity* Entity, const glm::vec3& Rotation, float Distance) const; // Returns true if movement occurred without voxel collision - not thread safe

        void MoveEntity(glm::vec3 Translation, SnazzCraft::Entity* Entity) const; // Returns true if movement occurred without voxel collision - not thread safe

        bool SaveWorldToFile(bool OverwriteExistingFile); 

        inline void ApplyGravityToEntities(std::vector<SnazzCraft::Entity*> AdditionalEntities)
        {
            constexpr float MoveDistance = 0.2f;

            for (auto& ChunkPair : this->Chunks) {
                for (SnazzCraft::Entity* Entity : ChunkPair.second->Entities) {
                    this->MoveEntity(glm::vec3(0.0f, -MoveDistance, 0.0f), Entity);
                }
            }

            for (SnazzCraft::Entity* Entity : AdditionalEntities) {
                this->MoveEntity(glm::vec3(0.0f, -MoveDistance, 0.0f), Entity);
            }
        }

        /*
        Calls UpdateVerticesAndIndices & UpdateMesh on all chunks affected
        If the Chunk in the address given has no light producing voxels then no member functions of that chunk will be called to update its vertices, indices, or mesh
        Not thread safe
        */
        void UpdateChunkLighting(SnazzCraft::Chunk* Chunk, bool* UpatedInputChunk);
        

    private:
        struct LightNode
        {
            union 
            {
                struct 
                {
                    int32_t X, Y, Z;
                };
                int32_t Position[3];
            };
            int8_t LightValue;

            LightNode(int8_t ILightValue, int32_t IX, int32_t IY, int32_t IZ);

            LightNode(int8_t ILightValue, int32_t IPosition[3]);
        };

        SnazzCraft::HeightMap* WorldHeightMap = nullptr;

        void ApplySunLightingToChunk(SnazzCraft::Chunk* Chunk, std::unordered_set<uint32_t>& ChunksToUpdate);

        void ApplySunLightingToColumn(SnazzCraft::Chunk* Chunk, uint32_t LocalChunkX, uint32_t LocalChunkZ);
        
        /*
        Only to be called trough UpdateChunkLighting
        Generates currently ungenerated Chunks when light values would affect them
        Not Thread safe
        */
        void ApplyLightingVoxel(int32_t LightOrigin[3], int32_t LightProducingLevel, std::unordered_set<uint32_t>& ChunksToUpdate);
        
    public:
        static SnazzCraft::World* CreateWorld(std::string Name, uint32_t Size, int32_t Seed);

        static SnazzCraft::World* LoadWorldFromSaveFile(std::string FilePath);
    };
    
    extern SnazzCraft::World* CurrentWorld;
} // SnazzCraft
