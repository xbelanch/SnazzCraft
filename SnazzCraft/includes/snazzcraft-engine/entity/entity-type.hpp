#pragma once

#include <stdint.h>

#include "snazzcraft-engine/mesh/mesh.hpp"
#include "snazzcraft-engine/texture/texture.hpp"
#include "snazzcraft-engine/entity/entity-ids.h"
#include "snazzcraft-engine/hitbox/hitbox.hpp"

namespace SnazzCraft
{
    class EntityType
    {
    public:
        SnazzCraft::Mesh* EntityMesh;
        SnazzCraft::Texture* EntityTexture;
        SnazzCraft::Hitbox* EntityHitbox;

        static void Initialize();

        static void FreeResources();
        
        static const EntityType& GetEntityType(uint8_t ID);

    private:
        EntityType(SnazzCraft::Mesh* IEntityMesh, SnazzCraft::Texture* IEntityTexture, SnazzCraft::Hitbox* IEntityHitbox);

    };


} // SnazzCraft