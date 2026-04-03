#pragma once

#include <vector>
#include <stdint.h>

#include "glm/glm.hpp"
#include "snazzcraft-engine/world/voxel-ids.h"

#define MAX_VOXEL_LIGHT_VALUE (20)

namespace SnazzCraft
{
    class Voxel
    {
    public:
        static constexpr uint32_t Size = 2;

        uint32_t Position[3]; // In local chunk space
        uint32_t ID;

        // Specific values ->
        int LightProducingLevel = 0;
        bool Cullable = true;
        bool CollidableToEntities = true;
        bool CollidableToVoxels = true;
        // <-

        bool Sides[6] = { true, true, true, true, true, true }; // Front, Left, Right, Back, Top, Bottom

        Voxel(uint32_t X, uint32_t Y, uint32_t Z, uint32_t ID);

        inline void AutoSetSpecificValues()
        {
            switch (this->ID)
            {
                case ID_VOXEL_ABOVE_GRASS:
                    this->Cullable = false;
                    this->CollidableToEntities = false;
                    this->CollidableToVoxels = true;
                    break;

                case ID_VOXEL_TORCH:
                this->LightProducingLevel = 18;
                    this->Cullable = false;
                    this->CollidableToEntities = false;
                    this->CollidableToVoxels = true;
                    break;

                default:
                    this->Cullable = true;
                    this->CollidableToEntities = true;
                    this->CollidableToVoxels = true;
            }
        }

        inline uint32_t GetSideCount() const
        {
            uint32_t Count = 0;
            for (uint32_t I = 0; I < 6; I++) {
                if (this->Sides[I]) Count++;
            }

            return Count;
        }

    private:

    };
} // SnazzCraft

