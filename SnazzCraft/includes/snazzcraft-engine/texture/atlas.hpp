#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "../vertice/voxel-vertice.hpp"
#include "../world/voxel.hpp"
#include "../world/voxel-ids.h"
#include "../mesh/mesh.hpp"
#include "snazzcraft-engine/utilities/math.hpp"

#define ATLAS_SIZE (5)
#define VOXEL_TEXTURE_SIZE_PIXELS (256)
#define ATLAS_TILE_SIZE (1.0f / (float)ATLAS_SIZE)
#define ATLAS_SIZE_PIXELS (ATLAS_SIZE * VOXEL_TEXTURE_SIZE_PIXELS)
#define HALF_PIXEL (0.5f / (256 * ATLAS_SIZE))

#define TEXTURE_COORDINATE_TORCH_OFFSET_X (112.0f / 256.0f)
#define TEXTURE_COORDINATE_TORCH_OFFSET_Y (96.0f / 256.0f)

namespace SnazzCraft
{
    class VoxelTextureApplier
    {
    public:
        VoxelTextureApplier(const char* AtlasFilePath);

        ~VoxelTextureApplier();

        std::vector<SnazzCraft::VoxelVertice> GetTexturedVertices(const SnazzCraft::Voxel& Voxel);

    private:
        unsigned int TextureCoordinates[ATLAS_SIZE * ATLAS_SIZE][2];
        unsigned int TextureCoordinatesCount = 0;

        bool LoadAtlasCoordinates(const char* AtlasFilePath);
    };

    extern SnazzCraft::VoxelTextureApplier* EngineVoxelTextureApplier;
} // SnazzCraft

