#pragma once

#include "snazzcraft-engine/world/voxel.hpp"

namespace SnazzCraft
{
    class VoxelType
    {
    public:
        int32_t LightProducingLevel;
        int32_t LightPropogationDecrease;
        uint8_t CullableSides;
        bool CollidableToEntities;
        bool CollidableToVoxels;

        /*
        Bit order of CullableSides:
            false, false, Bottom, Top, Back, Right, Left, Front
        */

        static const VoxelType& GetVoxelType(uint8_t VoxelID);

    private:
        VoxelType(int32_t ILightProducingLevel, int32_t ILightPropogationDecrease, uint8_t ICullableSides, bool ICollidableToEntities, bool ICollidableToVoxels);
    };
} // SnazzCraft
