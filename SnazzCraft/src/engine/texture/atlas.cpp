#include "snazzcraft-engine/texture/atlas.hpp"

SnazzCraft::VoxelTextureApplier* SnazzCraft::EngineVoxelTextureApplier = new SnazzCraft::VoxelTextureApplier("textures/voxel/atlas-coordinates.txt");

SnazzCraft::VoxelTextureApplier::VoxelTextureApplier(const char* AtlasFilePath) 
{ 
    this->LoadAtlasCoordinates(AtlasFilePath);
}

SnazzCraft::VoxelTextureApplier::~VoxelTextureApplier() 
{ 

}

std::vector<SnazzCraft::Vertice3D> SnazzCraft::VoxelTextureApplier::GetTexturedVertices(const SnazzCraft::Voxel& Voxel)
{
    const glm::vec2 HalfPixel(HALF_PIXEL, HALF_PIXEL);   
    const int HalfVoxelSize = static_cast<int>(SnazzCraft::Voxel::Size / 2.0f);

    std::vector<SnazzCraft::Vertice3D> Vertices;

    glm::vec2 AtlasCoordinates = { 
        (float)this->TextureCoordinates[Voxel.ID][0],
        (float)this->TextureCoordinates[Voxel.ID][1]
    };

    AtlasCoordinates /= (float)ATLAS_SIZE; 

    for (const SnazzCraft::Vertice3D& V : SnazzCraft::VoxelMesh->Vertices) { // Currently vertices are in mesh space
        Vertices.push_back(SnazzCraft::Vertice3D((V.Position * SnazzCraft::VoxelMesh->ScaleVector), V.TextureCoordinate + AtlasCoordinates));
    }

    switch (Voxel.ID) // Special cases
    {
        case ID_VOXEL_DIRT_GRASS_MIX:
        {
            // Update Texture Coordinates
            Vertices[16].TextureCoordinate = { 0.4f, 0.2f };
            Vertices[17].TextureCoordinate = { 0.4f, 0.0f };
            Vertices[18].TextureCoordinate = { 0.6f, 0.0f };
            Vertices[19].TextureCoordinate = { 0.6f, 0.2f };

            Vertices[20].TextureCoordinate = { 0.6f, 0.2f };
            Vertices[21].TextureCoordinate = { 0.6f, 0.0f };
            Vertices[22].TextureCoordinate = { 0.8f, 0.0f };
            Vertices[23].TextureCoordinate = { 0.8f, 0.2f };

            break;
        }

        case ID_VOXEL_TORCH:
        {
            // Update Texture Coordinates
            for (unsigned int I = 0; I < 6; I++) {
                unsigned int Index = I * 4;

                Vertices[Index + 0].TextureCoordinate.x += TEXTURE_COORDINATE_TORCH_OFFSET_X * ATLAS_TILE_SIZE;

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
            
            // Update Positions
            for (unsigned int I = 0; I < 24; I++) { 
                Vertices[I].Position.x = Vertices[I].Position.x < 0.0f ? -0.125f * HalfVoxelSize : 0.125f * HalfVoxelSize;
                Vertices[I].Position.y = Vertices[I].Position.y > 0.0f ?  0.75f  * HalfVoxelSize : Vertices[I].Position.y; 
                Vertices[I].Position.z = Vertices[I].Position.z < 0.0f ? -0.125f * HalfVoxelSize : 0.125f * HalfVoxelSize;
            }

            break;
        }

        default:
            break;
    }
    return Vertices;
}

bool SnazzCraft::VoxelTextureApplier::LoadAtlasCoordinates(const char* AtlasFilePath)
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

    return true;
}
