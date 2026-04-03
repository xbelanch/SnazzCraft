#include "snazzcraft-engine/world/voxel.hpp"
#include "snazzcraft-engine/world/voxel-type.hpp"

SnazzCraft::Voxel::Voxel(uint8_t IX, uint8_t IY, uint8_t IZ, uint32_t IID)
    : X(IX), Y(IY), Z(IZ), ID(IID)
{

}

const SnazzCraft::VoxelType& SnazzCraft::Voxel::GetVoxelType() const
{
    return SnazzCraft::VoxelType::GetVoxelType(this->ID);
}
