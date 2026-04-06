#include "snazzcraft-engine/entity/entity.hpp"

SnazzCraft::Entity::Entity(glm::vec3 IPosition, glm::vec3 IRotation, uint8_t IID)
    : Position(IPosition), Rotation(IRotation), ID(IID)
{
    this->Front = SnazzCraft::CalculateFrontVector(IRotation, true);
}

SnazzCraft::Entity::~Entity()
{
    
}

void SnazzCraft::Entity::Move(const glm::vec3& AdditionalRotation, float Distance)
{
    SnazzCraft::MoveVector3D(this->Position, this->Rotation + AdditionalRotation, Distance);
}

void SnazzCraft::Entity::Rotate(const glm::vec3& AdditionalRotation)
{
    for (unsigned int I = 0; I < 3; I++) {
        this->Rotation[I] += AdditionalRotation[I];
        this->Rotation[I] += this->Rotation[I] >  180.0f ? -360.0f : 0.0f;
        this->Rotation[I] += this->Rotation[I] < -180.0f ?  360.0f : 0.0f;
    }

    this->Front = SnazzCraft::CalculateFrontVector(this->Rotation, true);
}