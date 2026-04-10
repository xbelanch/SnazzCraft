#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "snazzcraft-engine/vertice/voxel-vertice.hpp"
#include "snazzcraft-engine/world/voxel.hpp"

#define ATLAS_SIZE (5)
#define VOXEL_TEXTURE_SIZE_PIXELS (256)
#define ATLAS_TILE_SIZE (1.0f / (float)ATLAS_SIZE)
#define ATLAS_SIZE_PIXELS (ATLAS_SIZE * VOXEL_TEXTURE_SIZE_PIXELS)
#define HALF_PIXEL (0.5f / ATLAS_SIZE_PIXELS)

#define TEXTURE_COORDINATE_TORCH_OFFSET_X (112.0f / (float)VOXEL_TEXTURE_SIZE_PIXELS)
#define TEXTURE_COORDINATE_TORCH_OFFSET_Y (96.0f / (float)VOXEL_TEXTURE_SIZE_PIXELS)

namespace SnazzCraft
{
    class VoxelTextureApplier
    {
    public:
        VoxelTextureApplier(const char* AtlasFilePath);

        ~VoxelTextureApplier() = default;

        std::vector<SnazzCraft::VoxelVertice> GetTexturedVertices(const SnazzCraft::Voxel& Voxel);

    private:
        unsigned int TextureCoordinates[ATLAS_SIZE * ATLAS_SIZE][2];
        unsigned int TextureCoordinatesCount = 0;

        void LoadAtlasCoordinates(const char* AtlasFilePath);
    };

    extern SnazzCraft::VoxelTextureApplier* EngineVoxelTextureApplier;
} // SnazzCraft

