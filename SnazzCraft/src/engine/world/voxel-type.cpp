#include "snazzcraft-engine/world/voxel-type.hpp"
#include "snazzcraft-engine/world/voxel.hpp"

const SnazzCraft::VoxelType& SnazzCraft::VoxelType::GetVoxelType(uint8_t VoxelID)
{
    static const SnazzCraft::VoxelType Phantom(0, 0, 0x00, false, false);
    static const SnazzCraft::VoxelType VoxelCollidable(0, 0, 0x00, false, true);
    static const SnazzCraft::VoxelType Torch(18, 0, 0x00, false, true);
    static const SnazzCraft::VoxelType Water(0, 1, 0x2F, true, false);
    static const SnazzCraft::VoxelType FullSolid(0, SnazzCraft::Voxel::MaxLightValue, 0x3F, true, true);

    switch (VoxelID)
    {
        case ID_VOXEL_WATER:
            return Water;

        case ID_VOXEL_ABOVE_GRASS:
            return VoxelCollidable;

        case ID_VOXEL_TORCH:
            return Torch;

        default:
            return FullSolid;
    }
}

SnazzCraft::VoxelType::VoxelType(int32_t ILightProducingLevel, int32_t ILightPropogationDecrease, uint8_t ICullableSides, bool ICollidableToEntities, bool ICollidableToVoxels)
    : LightProducingLevel(ILightProducingLevel), LightPropogationDecrease(ILightPropogationDecrease), 
      CullableSides(ICullableSides), CollidableToEntities(ICollidableToEntities), CollidableToVoxels(ICollidableToVoxels)
{

}


