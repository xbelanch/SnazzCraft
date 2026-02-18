#pragma once

#include "../../glm/glm.hpp"

namespace SnazzCraft
{
    class Hitbox
    {
    public:
        glm::vec3 Position;
        glm::vec3 HalfDimensions;

        Hitbox(glm::vec3 Position, glm::vec3 Dimensions);

        inline bool IsColliding(const Hitbox& CollideHitbox)
        {
            return (glm::abs(this->Position.x - CollideHitbox.Position.x) <= (this->HalfDimensions.x + CollideHitbox.HalfDimensions.x)) &&
                   (glm::abs(this->Position.y - CollideHitbox.Position.y) <= (this->HalfDimensions.y + CollideHitbox.HalfDimensions.y)) &&
                   (glm::abs(this->Position.z - CollideHitbox.Position.z) <= (this->HalfDimensions.z + CollideHitbox.HalfDimensions.z));
        }

        inline bool IsColliding(const glm::vec3& Point)
        {
            return (glm::abs(this->Position.x - Point.x) <= this->HalfDimensions.x) &&
                   (glm::abs(this->Position.y - Point.y) <= this->HalfDimensions.y) &&
                   (glm::abs(this->Position.z - Point.z) <= this->HalfDimensions.z);
        }

        constexpr void UpdateDimensions(const glm::vec3& NewDimensions)
        {
            this->HalfDimensions = NewDimensions / 2.0f;
        }

        constexpr glm::vec3 GetFullDimensions() const
        {
            return this->HalfDimensions * 2.0f;
        }

    private:



    };
}