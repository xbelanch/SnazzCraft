#include "user.hpp"

SnazzCraft::User* SnazzCraft::Player = new SnazzCraft::User({ 5.0f, 60.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });

SnazzCraft::User::User(glm::vec3 Position, glm::vec3 Rotation) : SnazzCraft::Entity(Position, Rotation)
{

}

void SnazzCraft::User::Move(const glm::vec3& Rotation, float Distance, bool UpdateHitbox) 
{
    this->Position += glm::vec3(Distance, 0.0f, Distance) * SnazzCraft::CalculateFrontVector(this->Rotation + Rotation, true);

    if (this->EntityHitbox != nullptr && UpdateHitbox) this->EntityHitbox->UpdatePosition(this->Position);
}

void SnazzCraft::User::Rotate(const glm::vec3& Rotation, bool UpdateHitbox) 
{
    this->Rotation[0] += Rotation[0];
    this->Rotation[0] = this->Rotation[0] >  180.0f ? this->Rotation[0] - 360.0f : this->Rotation[0];
    this->Rotation[0] = this->Rotation[0] < -180.0f ? this->Rotation[0] + 360.0f : this->Rotation[0];

    this->Rotation[1] += Rotation[1];
    this->Rotation[1] = this->Rotation[1] >  180.0f ? this->Rotation[1] - 360.0f : this->Rotation[1];
    this->Rotation[1] = this->Rotation[1] < -180.0f ? this->Rotation[1] + 360.0f : this->Rotation[1];

    this->Rotation[2] += Rotation[2];
    this->Rotation[2] = this->Rotation[2] >  89.99f ?  89.99f : this->Rotation[2];
    this->Rotation[2] = this->Rotation[2] < -89.99f ? -89.99f : this->Rotation[2];
    
    this->Front = SnazzCraft::CalculateFrontVector(this->Rotation, true);

    if (this->EntityHitbox != nullptr && UpdateHitbox) this->EntityHitbox->UpdateRotation(this->Rotation);
}
