#include "entity.hpp"

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

void SnazzCraft::Entity::Move(const glm::vec3& Rotation, float Distance, bool UpdateHitbox)
{
    this->Position += glm::vec3(Distance, Distance, Distance) * SnazzCraft::CalculateFrontVector(this->Rotation + Rotation, true);

    if (this->EntityHitbox != nullptr && UpdateHitbox) this->EntityHitbox->UpdatePosition(this->Position);
}

void SnazzCraft::Entity::Rotate(const glm::vec3& Rotation, bool UpdateHitbox)
{
    for (unsigned int I = 0; I < 3; I++) {
        this->Rotation[I] += Rotation[I];
        this->Rotation[I] = this->Rotation[I] >  180.0f ? this->Rotation[I] - 360.0f : this->Rotation[I];
        this->Rotation[I] = this->Rotation[I] < -180.0f ? this->Rotation[I] + 360.0f : this->Rotation[I];
    }

    this->Front = SnazzCraft::CalculateFrontVector(this->Rotation, true);

    if (this->EntityHitbox != nullptr && UpdateHitbox) this->EntityHitbox->UpdateRotation(this->Rotation);
}