#pragma once

#include "entity.hpp"

namespace SnazzCraft
{
    class User : public SnazzCraft::Entity
    {
    public:

    User(glm::vec3 Position, glm::vec3 Rotation);
    
    virtual void Move(const glm::vec3& AdditionalRotation, float Distance) override;

    virtual void Rotate(const glm::vec3& AdditionalRotation) override;

    private:
    

    };

    extern SnazzCraft::User* Player;
} // SnazzCraft
