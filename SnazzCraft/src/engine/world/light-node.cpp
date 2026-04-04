#include "snazzcraft-engine/world/world.hpp"

SnazzCraft::World::LightNode::LightNode(int8_t ILightValue, int32_t IX, int32_t IY, int32_t IZ)
    : X(IX), Y(IY), Z(IZ), LightValue(ILightValue)
{

}

SnazzCraft::World::LightNode::LightNode(int8_t ILightValue, int32_t IPosition[3])
    : LightValue(ILightValue)
{
    this->Position[0] = IPosition[0];
    this->Position[1] = IPosition[1];
    this->Position[2] = IPosition[2];
}