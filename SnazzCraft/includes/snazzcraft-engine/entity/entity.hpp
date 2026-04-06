#pragma once

#include "../../glm/glm.hpp"

#include "../mesh/mesh.hpp"
#include "../utilities/math.hpp"
#include "../hitbox/hitbox.hpp"
#include "snazzcraft-engine/entity/entity-ids.h"
#include "snazzcraft-engine/entity/entity-type.hpp"

namespace SnazzCraft
{
    class Entity
    {
    public:
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Front;

        uint8_t ID;

        Entity(glm::vec3 IPosition, glm::vec3 IRotation, uint8_t IID);

        virtual ~Entity();

        virtual void Move(const glm::vec3& AdditionalRotation, float Distance);

        virtual void Rotate(const glm::vec3& AdditionalRotation);

        inline const SnazzCraft::EntityType& GetEntityType()
        {
            return SnazzCraft::EntityType::GetEntityType(this->ID);
        }

    private:
        

        
    };

    
} // SnazzCraft

