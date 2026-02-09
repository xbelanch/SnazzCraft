#include "user.hpp"

SnazzCraft::User* SnazzCraft::Player = new SnazzCraft::User({ 10.0f, 60.0f, 10.0f }, { 0.0f, 0.0f, 0.0f });

SnazzCraft::User::User(glm::vec3 Position, glm::vec3 Rotation) : SnazzCraft::Entity(Position, Rotation)
{

}

void SnazzCraft::User::Move(const glm::vec3& AdditionalRotation, float Distance) 
{
    float PreviousY = this->Position.y;
    MoveVector3D(this->Position, this->Rotation + AdditionalRotation, Distance);
    this->Position.y = PreviousY;
    
    if (this->EntityHitbox != nullptr) this->EntityHitbox->Position = this->Position;
}

void SnazzCraft::User::Rotate(const glm::vec3& AdditionalRotation) 
{
    this->Rotation[0] += AdditionalRotation[0];
    this->Rotation[0] = this->Rotation[0] >  180.0f ? this->Rotation[0] - 360.0f : this->Rotation[0];
    this->Rotation[0] = this->Rotation[0] < -180.0f ? this->Rotation[0] + 360.0f : this->Rotation[0];

    this->Rotation[1] += AdditionalRotation[1];
    this->Rotation[1] = this->Rotation[1] >  180.0f ? this->Rotation[1] - 360.0f : this->Rotation[1];
    this->Rotation[1] = this->Rotation[1] < -180.0f ? this->Rotation[1] + 360.0f : this->Rotation[1];

    this->Rotation[2] += AdditionalRotation[2];
    this->Rotation[2] = this->Rotation[2] >  89.99f ?  89.99f : this->Rotation[2];
    this->Rotation[2] = this->Rotation[2] < -89.99f ? -89.99f : this->Rotation[2];
    
    this->Front = SnazzCraft::CalculateFrontVector(this->Rotation, true);
}
