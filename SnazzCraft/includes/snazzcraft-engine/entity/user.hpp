#pragma once

#include "entity.hpp"

extern const glm::vec3 UserHitboxDimensions;

namespace SnazzCraft
{
    class User : public SnazzCraft::Entity
    {
    public:

    User(glm::vec3 Position, glm::vec3 Rotation);
    
    void Move(const glm::vec3& AdditionalRotation, float Distance) override;

    void Rotate(const glm::vec3& AdditionalRotation) override;

    private:
    

    };

    extern SnazzCraft::User* Player;
}
