#include "snazzcraft-engine/entity/entity.hpp"

SnazzCraft::Entity::Entity(glm::vec3 Position, glm::vec3 Rotation)
{
    this->Position = Position;
    this->Rotation = Rotation;

    this->Front = SnazzCraft::CalculateFrontVector(this->Rotation, true);
}

SnazzCraft::Entity::~Entity()
{
    delete this->EntityMesh;
    delete this->EntityHitbox;
}

void SnazzCraft::Entity::Move(const glm::vec3& AdditionalRotation, float Distance)
{
    MoveVector3D(this->Position, this->Rotation + AdditionalRotation, Distance);

    if (this->EntityHitbox != nullptr) this->EntityHitbox->Position = this->Position + this->HitboxOffset;
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