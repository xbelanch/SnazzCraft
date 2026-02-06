#pragma once

#include "../entity.hpp"

namespace SnazzCraft
{
    class User : public SnazzCraft::Entity
    {
    public:

    User(glm::vec3 Position, glm::vec3 Rotation);
    
    void Move(const glm::vec3& Rotation, float Distance, bool UpdateHitbox) override;

    void Rotate(const glm::vec3& Rotation, bool UpdateHitbox) override;

    private:
    

    };

    extern SnazzCraft::User* Player;
}
