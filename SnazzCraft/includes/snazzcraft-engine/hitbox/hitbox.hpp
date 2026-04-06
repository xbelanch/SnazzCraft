#pragma once

#include "glm/glm.hpp"

namespace SnazzCraft
{
    class Hitbox
    {
    public:
        glm::vec3 HalfDimensions;

        Hitbox(glm::vec3 Dimensions);

        inline bool IsColliding(const glm::vec3& Position, const Hitbox& CollideHitbox, const glm::vec3& CollideHitboxPosition)
        {
            return (glm::abs(Position.x - CollideHitboxPosition.x) <= (this->HalfDimensions.x + CollideHitbox.HalfDimensions.x)) &&
                   (glm::abs(Position.y - CollideHitboxPosition.y) <= (this->HalfDimensions.y + CollideHitbox.HalfDimensions.y)) &&
                   (glm::abs(Position.z - CollideHitboxPosition.z) <= (this->HalfDimensions.z + CollideHitbox.HalfDimensions.z));
        }

        inline bool IsColliding(const glm::vec3& Position, const glm::vec3& Point)
        {
            return (glm::abs(Position.x - Point.x) <= this->HalfDimensions.x) &&
                   (glm::abs(Position.y - Point.y) <= this->HalfDimensions.y) &&
                   (glm::abs(Position.z - Point.z) <= this->HalfDimensions.z);
        }

        inline void UpdateDimensions(const glm::vec3& NewDimensions)
        {
            this->HalfDimensions = NewDimensions / 2.0f;
        }

        inline glm::vec3 GetFullDimensions() const
        {
            return this->HalfDimensions * 2.0f;
        }

    private:



    };
} // SnazzCraft