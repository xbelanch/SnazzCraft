#pragma once

#include <unordered_map>
#include <vector>
#include <array>
#include <mutex>

#include "../../glm/glm.hpp"

#include "../mesh/mesh.hpp"
#include "voxel.hpp"
#include "voxel-ids.h"
#include "../texture/atlas.hpp"
#include "../height-map/height-map.hpp"
#include "../utilities/math.hpp"
#include "../hitbox/hitbox.hpp"
#include "../entity/entity.hpp"

namespace SnazzCraft
{
    constexpr char VoxelCheckPositions[6][3] = {
        {  0,  0, -1 }, // Front
        { -1,  0,  0 }, // Left
        {  1,  0,  0 }, // Right
        {  0,  0,  1 }, // Back
        {  0,  1,  0 }, // Top
        {  0, -1,  0 }  // Bottom
    };

    class Chunk
    {
    public:
        static constexpr int32_t Width  = 16;
        static constexpr int32_t Height = 256;
        static constexpr int32_t Depth  = 16;

        #define VALID_LOCAL_VOXEL_POSITION(X, Y, Z) ((X) >= 0 && (Y) >= 0 && (Z) >= 0 && (X) < SnazzCraft::Chunk::Width && (Y) < SnazzCraft::Chunk::Height && (Z) < SnazzCraft::Chunk::Depth)
        #define LOCAL_VOXEL_INDEX(X, Y, Z)          (INDEX_3D(X, Y, Z, SnazzCraft::Chunk::Width, SnazzCraft::Chunk::Height))

        int32_t Position[2]; // X & Z Chunk Coordinates
        glm::vec3 ChunkWorldOffset;

        SnazzCraft::Mesh* ChunkMesh = nullptr;

        std::unordered_map<uint32_t, SnazzCraft::Voxel>* Voxels          = new std::unordered_map<uint32_t, SnazzCraft::Voxel>();
        std::unordered_map<uint32_t, SnazzCraft::Voxel>* OptimizedVoxels = new std::unordered_map<uint32_t, SnazzCraft::Voxel>();
        std::unordered_map<uint32_t, int>*               LightValues     = new std::unordered_map<uint32_t, int>();
        std::vector<SnazzCraft::Entity*> Entities;

        std::vector<SnazzCraft::Vertice3D>* Vertices = nullptr;
        std::vector<uint32_t>* Indices = nullptr;

        Chunk(int32_t X, int32_t Y); // Chunk Coordinates 

        ~Chunk();

        void Generate(SnazzCraft::HeightMap* HeightMap, uint32_t MapWidth);

        void UpdateVerticesAndIndices();

        void CullVoxelFaces(); // Clears previously optimized voxels and repopulates the std::unordered_map

        bool VoxelTouchingChunkBorder(uint32_t VoxelIndex, uint32_t* BorderDirection) const;

        SnazzCraft::Voxel* GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox) const; // Returns nullptr if no collision

        SnazzCraft::Voxel* GetCollidingVoxel(const glm::vec3& Position) const;

        SnazzCraft::Voxel* GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox, int32_t LocalVoxelX, int32_t LocalVoxelY, int32_t LocalVoxelZ) const;

        inline void UpdateMesh()
        {
            delete this->ChunkMesh;
            if (this->Vertices->empty() || this->Indices->empty()) { this->ChunkMesh = nullptr; return; }

            this->ChunkMesh = new SnazzCraft::Mesh(*this->Vertices, *this->Indices);
        }

    private:
        void ApplyBrightnessToVertices(std::vector<SnazzCraft::Vertice3D>& Vertices, const SnazzCraft::Voxel& Voxel);

        inline glm::vec3 LocalVoxelPositionToWorldPosition(uint32_t X, uint32_t Y, uint32_t Z) const
        {
            return glm::vec3((float)X, (float)Y, (float)Z) * glm::vec3((float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size) + this->ChunkWorldOffset;
        }

        inline void WorldSpaceToVoxelSpace(const glm::vec3& WorldPosition, int32_t VoxelPosition[3]) const
        {
            glm::vec3 LocalPosition = WorldPosition - this->ChunkWorldOffset;

            VoxelPosition[0] = static_cast<int>(LocalPosition.x / SnazzCraft::Voxel::Size);
            VoxelPosition[1] = static_cast<int>(LocalPosition.y / SnazzCraft::Voxel::Size);
            VoxelPosition[2] = static_cast<int>(LocalPosition.z / SnazzCraft::Voxel::Size);
        }

        SnazzCraft::Hitbox* VoxelCollisionHitbox = nullptr;

    
    public:
        static inline void GetChunkPosition(const glm::vec3& Position, int32_t OutChunkPosition[2]) // Voxel space coordinates
        {
            OutChunkPosition[0] = static_cast<int>(Position.x) / SnazzCraft::Chunk::Width;
            OutChunkPosition[1] = static_cast<int>(Position.z) / SnazzCraft::Chunk::Depth;
        }

        static inline void GetChunkPosition(int32_t X, int32_t Z, int32_t OutChunkPosition[2]) // Voxel space coordinates
        {
            OutChunkPosition[0] = X / SnazzCraft::Chunk::Width;
            OutChunkPosition[1] = Z / SnazzCraft::Chunk::Depth;
        }

        static inline void GetLocalVoxelPosition(int32_t X, int32_t Y, int32_t Z, int32_t OutLocalChunkPosition[3]) // Voxel space coordinates
        {
            OutLocalChunkPosition[0] = X % SnazzCraft::Chunk::Width;
            OutLocalChunkPosition[1] = Y % SnazzCraft::Chunk::Height;
            OutLocalChunkPosition[2] = Z % SnazzCraft::Chunk::Depth;
        }

    };
}





