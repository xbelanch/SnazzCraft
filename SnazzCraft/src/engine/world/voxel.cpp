#include "snazzcraft-engine/world/voxel.hpp"

SnazzCraft::Voxel::Voxel(uint32_t X, uint32_t Y, uint32_t Z, uint32_t ID)
{
    this->Position[0] = X;
    this->Position[1] = Y;
    this->Position[2] = Z;
    this->ID = ID;
}

SnazzCraft::Voxel::Voxel(uint32_t X, uint32_t Y, uint32_t Z, uint32_t ID, bool Cullable)
{
    this->Position[0] = X;
    this->Position[1] = Y;
    this->Position[2] = Z;
    this->ID = ID;
    this->Cullable = Cullable;
}

SnazzCraft::Voxel::Voxel(uint32_t X, uint32_t Y, uint32_t Z, uint32_t ID, bool Cullable, bool Collidable)
{
    this->Position[0] = X;
    this->Position[1] = Y;
    this->Position[2] = Z;
    this->ID = ID;
    this->Cullable = Cullable;
    this->Collidable = Collidable;
}


