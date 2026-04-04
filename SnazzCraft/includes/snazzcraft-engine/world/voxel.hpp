#pragma once

#include <vector>
#include <stdint.h>

#include "glm/glm.hpp"
#include "snazzcraft-engine/world/voxel-ids.h"
#include "snazzcraft-engine/world/voxel-type.hpp"
#include "snazzcraft-engine/utilities/byte-handling.hpp"

namespace SnazzCraft
{
    class VoxelType;

    class Voxel
    {
    public:
        static constexpr uint32_t Size = 2;
        static constexpr int32_t MaxLightValue = 20;

        union 
        {
            struct 
            {
                uint8_t X, Y, Z;
            };
            uint8_t Position[3]; // In local chunk space
        };
        uint8_t ID;

        Voxel(uint8_t IX, uint8_t IY, uint8_t IZ, uint8_t IID);

        const SnazzCraft::VoxelType& GetVoxelType() const;

        inline bool HasSide(uint8_t SideIndex) const
        {
            return SnazzCraft::AccessBitValue(this->Sides, SideIndex);
        }

        inline void ChangeSideValue(uint8_t SideIndex, bool Value)
        {
            SnazzCraft::ChangeValueInByte(this->Sides, SideIndex, Value);
        }

        inline void SetAllSides()
        {
            this->Sides = 0x3F;
        }

        inline void ClearAllSides()
        {
            this->Sides = 0x00;
        }

        inline uint8_t GetSideCount() const
        {
            uint8_t Count = 0;
            for (uint8_t I = 0; I < 6; I++) {
                if (this->HasSide(I)) Count++;
            }

            return Count;
        }

    private:
        // Bit order:
        // false, false, Bottom, Top, Back, Right, Left, Front
        uint8_t Sides = 0x3F;

    };
} // SnazzCraft

