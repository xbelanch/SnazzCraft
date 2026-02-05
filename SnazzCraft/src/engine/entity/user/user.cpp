#include "user.hpp"

SnazzCraft::User* SnazzCraft::Player = new SnazzCraft::User({ 5.0f, 60.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });

SnazzCraft::User::User(glm::vec3 Position, glm::vec3 Rotation) : SnazzCraft::Entity({ 20.0f, 50.0f, 20.0f }, { 0.0f, 0.0f, 0.0f })
{

}

void SnazzCraft::User::Move(const glm::vec3& Rotation, float Distance) 
{
    this->Position += glm::vec3(Distance, 0.0f, Distance) * SnazzCraft::CalculateFrontVector(this->Rotation + Rotation);

    if (this->EntityHitbox != nullptr) this->EntityHitbox->UpdatePosition(this->Position);
}

void SnazzCraft::User::Rotate(const glm::vec3& Rotation) 
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
    
    this->Front = SnazzCraft::CalculateFrontVector(this->Rotation);

    if (this->EntityHitbox != nullptr) this->EntityHitbox->UpdateRotation(this->Rotation);
}
