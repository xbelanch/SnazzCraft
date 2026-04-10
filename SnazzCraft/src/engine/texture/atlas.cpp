#include "snazzcraft-engine/texture/atlas.hpp"
#include "snazzcraft-engine/world/voxel-ids.h"
#include "snazzcraft-engine/mesh/mesh.hpp"

SnazzCraft::VoxelTextureApplier* SnazzCraft::EngineVoxelTextureApplier = new SnazzCraft::VoxelTextureApplier("textures/voxel/atlas-coordinates.txt");

SnazzCraft::VoxelTextureApplier::VoxelTextureApplier(const char* AtlasFilePath) 
{ 
    this->LoadAtlasCoordinates(AtlasFilePath);
}

std::vector<SnazzCraft::VoxelVertice> SnazzCraft::VoxelTextureApplier::GetTexturedVertices(const SnazzCraft::Voxel& Voxel)
{
    constexpr glm::vec2 HalfPixel(HALF_PIXEL, HALF_PIXEL);   
    constexpr int32_t HalfVoxelSize = static_cast<int32_t>(SnazzCraft::Voxel::Size / 2.0f);

    glm::vec2 AtlasCoordinates = { 
        (float)this->TextureCoordinates[Voxel.ID][0],
        (float)this->TextureCoordinates[Voxel.ID][1]
    };

    AtlasCoordinates /= (float)ATLAS_SIZE; 

    std::vector<SnazzCraft::VoxelVertice> Vertices;
    for (const SnazzCraft::VoxelVertice& V : SnazzCraft::VoxelMesh->Vertices) { // Currently vertices are in mesh space
        Vertices.push_back(SnazzCraft::VoxelVertice((V.Position * SnazzCraft::VoxelMesh->ScaleVector), V.TextureCoordinate + AtlasCoordinates));
    }

    switch (Voxel.ID) // Special cases
    {
        case ID_VOXEL_DIRT_GRASS_MIX:
        {
            Vertices[16].TextureCoordinate = { (ATLAS_TILE_SIZE * 2), (ATLAS_TILE_SIZE) };
            Vertices[17].TextureCoordinate = { (ATLAS_TILE_SIZE * 2), 0.0f };
            Vertices[18].TextureCoordinate = { (ATLAS_TILE_SIZE * 3), 0.0f };
            Vertices[19].TextureCoordinate = { (ATLAS_TILE_SIZE * 3), (ATLAS_TILE_SIZE) };

            Vertices[20].TextureCoordinate = { (ATLAS_TILE_SIZE * 3), (ATLAS_TILE_SIZE) };
            Vertices[21].TextureCoordinate = { (ATLAS_TILE_SIZE * 3), 0.0f };
            Vertices[22].TextureCoordinate = { (ATLAS_TILE_SIZE * 4), 0.0f };
            Vertices[23].TextureCoordinate = { (ATLAS_TILE_SIZE * 4), (ATLAS_TILE_SIZE) };

            break;
        }

        case ID_VOXEL_TORCH:
        {
            for (uint32_t I = 0; I < 6; I++) {
                uint32_t Index = I * 4;

                Vertices[Index]    .TextureCoordinate.x += TEXTURE_COORDINATE_TORCH_OFFSET_X * ATLAS_TILE_SIZE;

                Vertices[Index + 1].TextureCoordinate.x += TEXTURE_COORDINATE_TORCH_OFFSET_X * ATLAS_TILE_SIZE;
                Vertices[Index + 1].TextureCoordinate.y += TEXTURE_COORDINATE_TORCH_OFFSET_Y * ATLAS_TILE_SIZE;

                Vertices[Index + 2].TextureCoordinate.x -= TEXTURE_COORDINATE_TORCH_OFFSET_X * ATLAS_TILE_SIZE;
                Vertices[Index + 2].TextureCoordinate.y += TEXTURE_COORDINATE_TORCH_OFFSET_Y * ATLAS_TILE_SIZE;

                Vertices[Index + 3].TextureCoordinate.x -= TEXTURE_COORDINATE_TORCH_OFFSET_X * ATLAS_TILE_SIZE;
            }

            Vertices[16].TextureCoordinate.y -= ATLAS_TILE_SIZE * 0.5f;
            Vertices[19].TextureCoordinate.y -= ATLAS_TILE_SIZE * 0.5f;

            Vertices[21].TextureCoordinate.y += ATLAS_TILE_SIZE * 0.5f;
            Vertices[22].TextureCoordinate.y += ATLAS_TILE_SIZE * 0.5f;
            
            for (uint32_t I = 0; I < 24; I++) { 
                Vertices[I].Position.x = Vertices[I].Position.x < 0.0f ? -0.125f * HalfVoxelSize : 0.125f * HalfVoxelSize;
                Vertices[I].Position.y = Vertices[I].Position.y > 0.0f ?  0.75f  * HalfVoxelSize : Vertices[I].Position.y; 
                Vertices[I].Position.z = Vertices[I].Position.z < 0.0f ? -0.125f * HalfVoxelSize : 0.125f * HalfVoxelSize;
            }

            break;
        }

        case ID_VOXEL_OAK_LOG:
        {
            Vertices[16].TextureCoordinate = { (ATLAS_TILE_SIZE * 2), (ATLAS_TILE_SIZE * 4) };
            Vertices[17].TextureCoordinate = { (ATLAS_TILE_SIZE * 2), (ATLAS_TILE_SIZE * 3) };
            Vertices[18].TextureCoordinate = { (ATLAS_TILE_SIZE * 3), (ATLAS_TILE_SIZE * 3) };
            Vertices[19].TextureCoordinate = { (ATLAS_TILE_SIZE * 3), (ATLAS_TILE_SIZE * 4) };

            Vertices[20].TextureCoordinate = { (ATLAS_TILE_SIZE * 2), (ATLAS_TILE_SIZE * 4) };
            Vertices[21].TextureCoordinate = { (ATLAS_TILE_SIZE * 2), (ATLAS_TILE_SIZE * 3) };
            Vertices[22].TextureCoordinate = { (ATLAS_TILE_SIZE * 3), (ATLAS_TILE_SIZE * 3) };
            Vertices[23].TextureCoordinate = { (ATLAS_TILE_SIZE * 3), (ATLAS_TILE_SIZE * 4) };

            break;
        }
        
        default:
            break;
    }

    return Vertices;
}

void SnazzCraft::VoxelTextureApplier::LoadAtlasCoordinates(const char* AtlasFilePath)
{
    std::ifstream File(AtlasFilePath);
    std::string Line;

    this->TextureCoordinatesCount = 0;
    while (std::getline(File, Line) && this->TextureCoordinatesCount < ATLAS_SIZE * ATLAS_SIZE) 
    {
        if (Line.empty()) continue;

        std::stringstream SS(Line);
        SS >> this->TextureCoordinates[this->TextureCoordinatesCount][0] >> this->TextureCoordinates[this->TextureCoordinatesCount][1];

        this->TextureCoordinatesCount++;
    }

    File.close();
}