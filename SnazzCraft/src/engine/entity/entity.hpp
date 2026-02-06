#pragma once

#include "../../../includes/glm/glm.hpp"

#include "../mesh/mesh.hpp"
#include "../utilities/math/math.hpp"
#include "../hitbox/hitbox.hpp"

namespace SnazzCraft
{
    class Entity
    {
    public:
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Front;

        SnazzCraft::Mesh* EntityMesh = nullptr;
        SnazzCraft::Hitbox* EntityHitbox = nullptr;

        Entity(glm::vec3 Position, glm::vec3 Rotation);
        virtual ~Entity();

        virtual void Move(const glm::vec3& Rotation, float Distance, bool UpdateHitbox);

        virtual void Rotate(const glm::vec3& Rotation, bool UpdateHitbox);

        inline void SetHitbox(const glm::vec3& Dimensions)
        {
            delete this->EntityHitbox;
            this->EntityHitbox = new SnazzCraft::Hitbox(this->Position, Dimensions, this->Rotation);
        }

    private:
        

        
    };

    
}

