#include "snazzcraft-engine/entity/user.hpp"

SnazzCraft::User* SnazzCraft::Player = new SnazzCraft::User({ -5.0f, 70.0f, -50.0f }, { 0.0f, 0.0f, 0.0f });

SnazzCraft::User::User(glm::vec3 Position, glm::vec3 Rotation) : SnazzCraft::Entity(Position, Rotation, ID_ENTITY_PLAYER)
{
  
}

void SnazzCraft::User::Move(const glm::vec3& AdditionalRotation, float Distance) 
{
    SnazzCraft::MoveVector3D(this->Position, { this->Rotation.x + AdditionalRotation.x, this->Rotation.y + AdditionalRotation.y, 0.0f }, Distance);
}

void SnazzCraft::User::Rotate(const glm::vec3& AdditionalRotation) 
{
    this->Rotation[0] += AdditionalRotation[0];
    this->Rotation[0] += this->Rotation[0] >  180.0f ? -360.0f : 0.0f;
    this->Rotation[0] += this->Rotation[0] < -180.0f ?  360.0f : 0.0f;

    this->Rotation[1] += AdditionalRotation[1];
    this->Rotation[1] += this->Rotation[1] >  180.0f ? -360.0f : 0.0f;
    this->Rotation[1] += this->Rotation[1] < -180.0f ?  360.0f : 0.0f;

    this->Rotation[2] += AdditionalRotation[2];
    this->Rotation[2] = this->Rotation[2] >  89.999f ?  89.999f : this->Rotation[2];
    this->Rotation[2] = this->Rotation[2] < -89.999f ? -89.999f : this->Rotation[2];
    
    this->Front = SnazzCraft::CalculateFrontVector(this->Rotation, true);
}
