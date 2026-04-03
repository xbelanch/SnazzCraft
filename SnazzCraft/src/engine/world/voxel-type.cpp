#include "snazzcraft-engine/world/voxel-type.hpp"
#include "snazzcraft-engine/world/voxel.hpp"

SnazzCraft::VoxelType::VoxelType(int32_t ILightProducingLevel, int32_t ILightPropogationDecrease, bool ICullable, bool ICollidableToEntities, bool ICollidableToVoxels)
    : LightProducingLevel(ILightProducingLevel), LightPropogationDecrease(ILightPropogationDecrease), 
      Cullable(ICullable), CollidableToEntities(ICollidableToEntities), CollidableToVoxels(ICollidableToVoxels)
{

}

const SnazzCraft::VoxelType& SnazzCraft::VoxelType::GetVoxelType(uint32_t VoxelID)
{
    static SnazzCraft::VoxelType AboveGrass(0, 0, false, false, true);
    static SnazzCraft::VoxelType Torch(18, 0, false, false, true);
    static SnazzCraft::VoxelType FullSolid(0, SnazzCraft::Voxel::MaxLightValue, true, true, true);

    switch (VoxelID)
    {
        case ID_VOXEL_ABOVE_GRASS:
            return AboveGrass;

        case ID_VOXEL_TORCH:
            return Torch;

        default:
            return FullSolid;
    }
}
