#pragma once

#include <unordered_map>
#include <vector>
#include <array>

#include "../../../../includes/glm/glm.hpp"

#include "../../mesh/mesh.hpp"
#include "../voxel/voxel.hpp"
#include "../voxel/voxel-ids.h"
#include "../../texture/atlas/atlas.hpp"
#include "../../height-map/height-map.hpp"
#include "../../utilities/math/math.hpp"
#include "../../hitbox/hitbox.hpp"
#include "../../entity/entity.hpp"

namespace SnazzCraft
{
    class Chunk
    {
    public:
        const static int Width  = 16;
        const static int Height = 256;
        const static int Depth  = 16;

        #define VALID_VOXEL_POSITION(X, Y, Z) ((X) >= 0 && (Y) >= 0 && (Z) >= 0 && (X) < SnazzCraft::Chunk::Width && (Y) < SnazzCraft::Chunk::Height && (Z) < SnazzCraft::Chunk::Depth)
        #define VOXEL_INDEX(X, Y, Z) (INDEX_3D(X, Y, Z, SnazzCraft::Chunk::Width, SnazzCraft::Chunk::Height))

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

        void CullVoxelFaces(); // Clears previously optimized voxels and repopulates it

        bool VoxelTouchingChunkBorder(unsigned int VoxelIndex, unsigned int* BorderDirection);

        SnazzCraft::Voxel* IsCollidingVoxel(const SnazzCraft::Hitbox* Hitbox); // Returns nullptr if no collision

    private:
        void ApplyBrightnessToVertices(std::vector<SnazzCraft::Vertice3D>& Vertices, const SnazzCraft::Voxel& Voxel);

        inline glm::vec3 VoxelPositionToWorldPosition(unsigned int X, unsigned int Y, unsigned int Z) const
        {
            return glm::vec3((float)X, (float)Y, (float)Z) * glm::vec3((float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size) + this->ChunkWorldOffset;
        }

        inline void WorldSpaceToVoxelSpace(const glm::vec3& WorldPosition, int VoxelPosition[3]) 
        {
            glm::vec3 LocalPosition = WorldPosition - this->ChunkWorldOffset;

            VoxelPosition[0] = static_cast<int>(LocalPosition.x / SnazzCraft::Voxel::Size);
            VoxelPosition[1] = static_cast<int>(LocalPosition.y / SnazzCraft::Voxel::Size);
            VoxelPosition[2] = static_cast<int>(LocalPosition.z / SnazzCraft::Voxel::Size);
        }

        SnazzCraft::Hitbox* VoxelCollisionHitbox = nullptr;

    };

    extern const int VoxelCheckPositions[6][3];
}





