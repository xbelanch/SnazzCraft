#pragma once

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include "snazzcraft-engine/mesh/mesh.hpp"
#include "snazzcraft-engine/texture/texture.hpp"
#include "snazzcraft-engine/entity/entity-ids.h"
#include "snazzcraft-engine/hitbox/hitbox.hpp"
#include "snazzcraft-engine/utilities/text-file-handling.hpp"

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
        static SnazzCraft::Mesh* Meshes[256];
        static uint8_t MeshesLoaded;

        static SnazzCraft::Texture* Textures[256];
        static uint8_t TexturesLoaded;

        static SnazzCraft::Hitbox* Hitboxes[256];
        static uint8_t HitboxesLoaded;

        EntityType(SnazzCraft::Mesh* IEntityMesh, SnazzCraft::Texture* IEntityTexture, SnazzCraft::Hitbox* IEntityHitbox);

        static void LoadMeshes();

        static void LoadTextures();

        static void LoadHitboxes();

    };


} // SnazzCraft