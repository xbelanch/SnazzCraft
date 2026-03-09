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
    class Chunk
    {
    public:
        static constexpr int Width  = 16;
        static constexpr int Height = 256;
        static constexpr int Depth  = 16;

        #define VALID_LOCAL_VOXEL_POSITION(X, Y, Z) ((X) >= 0 && (Y) >= 0 && (Z) >= 0 && (X) < SnazzCraft::Chunk::Width && (Y) < SnazzCraft::Chunk::Height && (Z) < SnazzCraft::Chunk::Depth)
        #define LOCAL_VOXEL_INDEX(X, Y, Z)          (INDEX_3D(X, Y, Z, SnazzCraft::Chunk::Width, SnazzCraft::Chunk::Height))

        int Position[2]; // X & Z Chunk Coordinates
        glm::vec3 ChunkWorldOffset;

        SnazzCraft::Mesh* ChunkMesh = nullptr;

        std::unordered_map<unsigned int, SnazzCraft::Voxel>* Voxels          = new std::unordered_map<unsigned int, SnazzCraft::Voxel>();
        std::unordered_map<unsigned int, SnazzCraft::Voxel>* OptimizedVoxels = new std::unordered_map<unsigned int, SnazzCraft::Voxel>();
        std::unordered_map<unsigned int, int>*               LightValues     = new std::unordered_map<unsigned int, int>();
        std::vector<SnazzCraft::Entity*> Entities;

        Chunk(int X, int Y); // Chunk Coordinates 

        ~Chunk();

        void Generate(SnazzCraft::HeightMap* HeightMap, unsigned int MapWidth);

        void UpdateMesh();

        void CullVoxelFaces(); // Clears previously optimized voxels and repopulates the std::unordered_map

        bool VoxelTouchingChunkBorder(unsigned int VoxelIndex, unsigned int* BorderDirection) const;

        SnazzCraft::Voxel* GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox) const; // Returns nullptr if no collision

        SnazzCraft::Voxel* GetCollidingVoxel(const glm::vec3& Position) const;

        SnazzCraft::Voxel* GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox, int LocalVoxelX, int LocalVoxelY, int LocalVoxelZ) const;

    private:
        void ApplyBrightnessToVertices(std::vector<SnazzCraft::Vertice3D>& Vertices, const SnazzCraft::Voxel& Voxel) const;

        inline glm::vec3 LocalVoxelPositionToWorldPosition(unsigned int X, unsigned int Y, unsigned int Z) const
        {
            return glm::vec3((float)X, (float)Y, (float)Z) * glm::vec3((float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size) + this->ChunkWorldOffset;
        }

        inline void WorldSpaceToVoxelSpace(const glm::vec3& WorldPosition, int VoxelPosition[3]) const
        {
            glm::vec3 LocalPosition = WorldPosition - this->ChunkWorldOffset;

            VoxelPosition[0] = static_cast<int>(LocalPosition.x / SnazzCraft::Voxel::Size);
            VoxelPosition[1] = static_cast<int>(LocalPosition.y / SnazzCraft::Voxel::Size);
            VoxelPosition[2] = static_cast<int>(LocalPosition.z / SnazzCraft::Voxel::Size);
        }

        SnazzCraft::Hitbox* VoxelCollisionHitbox = nullptr;

    
    public:
        static inline void GetChunkPosition(const glm::vec3& Position, int OutChunkPosition[2]) // World space coordinates
        {
            OutChunkPosition[0] = static_cast<int>(Position.x) / SnazzCraft::Chunk::Width;
            OutChunkPosition[1] = static_cast<int>(Position.z) / SnazzCraft::Chunk::Depth;
        }

        static inline void GetChunkPosition(int X, int Z, int OutChunkPosition[2]) // Voxel space coordinates
        {
            OutChunkPosition[0] = X / SnazzCraft::Chunk::Width;
            OutChunkPosition[1] = Z / SnazzCraft::Chunk::Depth;
        }

        static inline void GetLocalVoxelPosition(int X, int Y, int Z, int OutLocalChunkPosition[3]) // Voxel space coordinates
        {
            OutLocalChunkPosition[0] = X % SnazzCraft::Chunk::Width;
            OutLocalChunkPosition[1] = Y % SnazzCraft::Chunk::Height;
            OutLocalChunkPosition[2] = Z % SnazzCraft::Chunk::Depth;
        }

    };

    extern const int VoxelCheckPositions[6][3];
}





