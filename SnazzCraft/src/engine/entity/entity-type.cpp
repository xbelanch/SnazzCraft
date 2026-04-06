#include "snazzcraft-engine/entity/entity-type.hpp"

const char* TexturesToIntialize[] = {
    "textures/solids/light-green.png",
    "textures/solids/dark-blue.png",
    nullptr
};
SnazzCraft::Texture* Textures[256];
uint8_t TexturesSet = 0x00;

const char* MeshesToInitialize[] = {
    "entity-meshes/test-cube.obj",
    "entity-meshes/test-rectangular-prism.obj",
    nullptr
};
SnazzCraft::Mesh* Meshes[256];
uint8_t MeshesSet = 0x00;

const glm::vec3 HumanoidHitboxDimensions = glm::vec3(1.75f, 5.75f, 1.75f);
SnazzCraft::Hitbox* HumanoidHitbox;

void SnazzCraft::EntityType::Initialize()
{
    uint8_t TextureToIntializeIndex = 0x00;
    while (TexturesToIntialize[TextureToIntializeIndex] != nullptr)
    {
        Textures[TextureToIntializeIndex] = new SnazzCraft::Texture(TexturesToIntialize[TextureToIntializeIndex]);

        TextureToIntializeIndex++;
        TexturesSet++;
    }

    uint8_t MeshesToInitializeIndex = 0x00;
    while (MeshesToInitialize[MeshesToInitializeIndex] != nullptr)
    {
        Meshes[MeshesToInitializeIndex] = SnazzCraft::Mesh::LoadMeshFromObjectFile(MeshesToInitialize[MeshesToInitializeIndex]);

        MeshesToInitializeIndex++;
        MeshesSet++;
    }

    HumanoidHitbox = new SnazzCraft::Hitbox(HumanoidHitboxDimensions);
}

void SnazzCraft::EntityType::FreeResources()
{
    for (uint8_t I = 0x00; I < TexturesSet; I++) {
        delete Textures[I];
    }

    for (uint8_t I = 0x00; I < MeshesSet; I++) {
        delete Meshes[I];
    }

    delete HumanoidHitbox;
}

const SnazzCraft::EntityType& SnazzCraft::EntityType::GetEntityType(uint8_t ID)
{
    static const SnazzCraft::EntityType Player(Meshes[ID_ENTITY_PLAYER], Textures[ID_ENTITY_PLAYER], HumanoidHitbox);
    static const SnazzCraft::EntityType Test(Meshes[ID_ENTITY_TEST], Textures[ID_ENTITY_TEST], HumanoidHitbox);

    switch (ID)
    {
        case ID_ENTITY_PLAYER:
            return Player;

        case ID_ENTITY_TEST:
            return Test;

        default:
            return Test;
    }
}

SnazzCraft::EntityType::EntityType(SnazzCraft::Mesh* IEntityMesh, SnazzCraft::Texture* IEntityTexture, SnazzCraft::Hitbox* IEntityHitbox)
    : EntityMesh(IEntityMesh), EntityTexture(IEntityTexture), EntityHitbox(IEntityHitbox)
{

}