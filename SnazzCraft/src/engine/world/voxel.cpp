#include "snazzcraft-engine/world/voxel.hpp"

SnazzCraft::Voxel::Voxel(unsigned int X, unsigned int Y, unsigned int Z, unsigned int ID)
{
    this->Position[0] = X;
    this->Position[1] = Y;
    this->Position[2] = Z;
    this->ID = ID;
}

SnazzCraft::Voxel::Voxel(unsigned int X, unsigned int Y, unsigned int Z, unsigned int ID, bool Cullable)
{
    this->Position[0] = X;
    this->Position[1] = Y;
    this->Position[2] = Z;
    this->ID = ID;
    this->Cullable = Cullable;
}

SnazzCraft::Voxel::Voxel(unsigned int X, unsigned int Y, unsigned int Z, unsigned int ID, bool Cullable, bool Collidable)
{
    this->Position[0] = X;
    this->Position[1] = Y;
    this->Position[2] = Z;
    this->ID = ID;
    this->Cullable = Cullable;
    this->Collidable = Collidable;
}


