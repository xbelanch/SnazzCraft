#include "snazzcraft-engine/entity/entity-type.hpp"

SnazzCraft::Mesh* SnazzCraft::EntityType::Meshes[256];
uint8_t SnazzCraft::EntityType::MeshesLoaded = 0x00;
#define MESH_TEST_CUBE              (0x00)
#define MESH_TEST_RECTANGULAR_PRISM (0x01)
#define MESH_CATTLE                 (0x02)

SnazzCraft::Texture* SnazzCraft::EntityType::Textures[256];
uint8_t SnazzCraft::EntityType::TexturesLoaded = 0x00;
#define TEXTURE_LIGHT_GREEN (0x00)
#define TEXTURE_DARK_BLUE   (0x01)
#define TEXTURE_WHITE       (0x02)

SnazzCraft::Hitbox* SnazzCraft::EntityType::Hitboxes[256];
uint8_t SnazzCraft::EntityType::HitboxesLoaded = 0x00;
#define HITBOX_HUMANONOID (0x00)
#define HITBOX_CATTLE     (0x01)

void SnazzCraft::EntityType::Initialize()
{
    SnazzCraft::EntityType::LoadMeshes();
    SnazzCraft::EntityType::LoadTextures();
    SnazzCraft::EntityType::LoadHitboxes();

    Meshes[MESH_CATTLE]->ScaleVector = glm::vec3(0.5f);
}

void SnazzCraft::EntityType::FreeResources()
{
    for (uint8_t I = 0x00; I < SnazzCraft::EntityType::MeshesLoaded; I++) {
        delete Meshes[I];
    }

    for (uint8_t I = 0x00; I < SnazzCraft::EntityType::TexturesLoaded; I++) {
        delete Textures[I];
    }

    for (uint8_t I = 0x00; I < SnazzCraft::EntityType::HitboxesLoaded; I++) {
        delete SnazzCraft::EntityType::Hitboxes[I];
    }
}

const SnazzCraft::EntityType& SnazzCraft::EntityType::GetEntityType(uint8_t ID)
{
    static const SnazzCraft::EntityType Player(Meshes[MESH_TEST_RECTANGULAR_PRISM], Textures[TEXTURE_LIGHT_GREEN], SnazzCraft::EntityType::Hitboxes[HITBOX_HUMANONOID]);
    static const SnazzCraft::EntityType Test  (Meshes[MESH_TEST_RECTANGULAR_PRISM], Textures[TEXTURE_DARK_BLUE],   SnazzCraft::EntityType::Hitboxes[HITBOX_HUMANONOID]);
    static const SnazzCraft::EntityType Sheep (Meshes[MESH_CATTLE],                 Textures[TEXTURE_WHITE],       SnazzCraft::EntityType::Hitboxes[HITBOX_CATTLE]);

    switch (ID)
    {
        case ID_ENTITY_PLAYER:
            return Player;

        case ID_ENTITY_TEST:
            return Test;

        case ID_ENTITY_SHEEP:
            return Sheep;

        default:
            return Test;
    }
}

SnazzCraft::EntityType::EntityType(SnazzCraft::Mesh* IEntityMesh, SnazzCraft::Texture* IEntityTexture, SnazzCraft::Hitbox* IEntityHitbox)
    : EntityMesh(IEntityMesh), EntityTexture(IEntityTexture), EntityHitbox(IEntityHitbox)
{

}

void SnazzCraft::EntityType::LoadMeshes()
{
    std::ifstream ListFile("entity-data-to-load/meshes.txt");
    if (!ListFile.is_open()) throw std::runtime_error("SNAZZCRAFT| Unable to open entity mesh list file\n");

    std::string ObjectFileToLoad;
    while (std::getline(ListFile, ObjectFileToLoad))
    {
        SnazzCraft::EntityType::Meshes[SnazzCraft::EntityType::MeshesLoaded++] = SnazzCraft::Mesh::LoadMeshFromObjectFile(ObjectFileToLoad.c_str());
    }

    ListFile.close();
}

void SnazzCraft::EntityType::LoadTextures()
{
    std::ifstream ListFile("entity-data-to-load/textures.txt");
    if (!ListFile.is_open()) throw std::runtime_error("SNAZZCRAFT| Unable to open entity texture list file\n");

    std::string TextureToLoad;
    while (std::getline(ListFile, TextureToLoad))
    {
        SnazzCraft::EntityType::Textures[SnazzCraft::EntityType::TexturesLoaded++] = new SnazzCraft::Texture(TextureToLoad.c_str());
    }

    ListFile.close();
}

void SnazzCraft::EntityType::LoadHitboxes()
{
    std::ifstream ListFile("entity-data-to-load/hitbox-dimensions.txt");
    if (!ListFile.is_open()) throw std::runtime_error("SNAZZCRAFT| Unable to open entity hitbox dimensions list file\n");

    char Space = ' ';
    std::string Line;
    float NewDimensions[3];
    while (std::getline(ListFile, Line))
    {
        uint32_t LineIndex = 0;
        for (uint8_t I = 0x00; I < 0x03; I++) {
            std::string NewValue;
            SnazzCraft::ParseData(NewValue, Line, LineIndex, &Space);

            NewDimensions[I] = std::stof(NewValue);
            LineIndex++;
        }

        SnazzCraft::EntityType::Hitboxes[SnazzCraft::EntityType::HitboxesLoaded++] = new SnazzCraft::Hitbox({ NewDimensions[0], NewDimensions[1], NewDimensions[2] });
    }

    ListFile.close();
}
