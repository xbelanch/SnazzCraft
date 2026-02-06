#include "chunk.hpp"

const int SnazzCraft::VoxelCheckPositions[6][3] = {
    {  0,  0, -1 }, // Front
    { -1,  0,  0 }, // Left
    {  1,  0,  0 }, // Right
    {  0,  0,  1 }, // Back
    {  0,  1,  0 }, // Top
    {  0, -1,  0 }  // Bottom
};

SnazzCraft::Chunk::Chunk(int X, int Y)
{
    this->Position[0] = X;
    this->Position[1] = Y;

    this->ChunkWorldOffset = {
        this->Position[0] * SnazzCraft::Chunk::Width * SnazzCraft::Voxel::Size,
        0.0f,
        this->Position[1] * SnazzCraft::Chunk::Depth * SnazzCraft::Voxel::Size 
    };
}

SnazzCraft::Chunk::~Chunk()
{
    delete this->Voxels;
    delete this->OptimizedVoxels;

    delete this->ChunkMesh;
}

void SnazzCraft::Chunk::Generate(SnazzCraft::HeightMap* HeightMap, unsigned int MapWidth)
{
    unsigned int HeightMapOffsetX = this->Position[0] * SnazzCraft::Chunk::Width;
    unsigned int HeightMapOffsetZ = this->Position[1] * SnazzCraft::Chunk::Depth;

    for (unsigned int X = 0; X < SnazzCraft::Chunk::Width; X++) {
    for (unsigned int Z = 0; Z < SnazzCraft::Chunk::Depth; Z++) {
        HeightMap->GenerateValue(X + HeightMapOffsetX, Z + HeightMapOffsetZ);
        auto HeightAtPositionIterator = HeightMap->HeightValues->find((Z + HeightMapOffsetZ) * MapWidth + (X + HeightMapOffsetX));

        for (unsigned int Y = 0; Y < HeightAtPositionIterator->second; Y++) {
            unsigned int NewVoxelID = ID_VOXEL_STONE;

            if (HeightAtPositionIterator->second != 0 && Y == HeightAtPositionIterator->second - 1) NewVoxelID = ID_VOXEL_DIRT_GRASS_MIX;
            else if (HeightAtPositionIterator->second != 0 && Y >= HeightAtPositionIterator->second - 4) NewVoxelID = ID_VOXEL_DIRT;

            this->Voxels->insert({ VOXEL_INDEX(X, Y, Z), SnazzCraft::Voxel(X, Y, Z, NewVoxelID) });
        }
    }
    }
}

void SnazzCraft::Chunk::UpdateMesh()
{
    delete this->ChunkMesh;

    std::vector<SnazzCraft::Vertice3D> NewTexturedVertices;
    std::vector<unsigned int> NewIndices;

    for (const auto& VoxelPair : *this->OptimizedVoxels) {
        glm::vec3 Offset = glm::vec3((float)VoxelPair.second.Position[0], (float)VoxelPair.second.Position[1], (float)VoxelPair.second.Position[2]) * glm::vec3((float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size) + this->ChunkWorldOffset; 
        unsigned int NewVerticesCount = 0;

        std::vector<SnazzCraft::Vertice3D> Vertices = SnazzCraft::EngineVoxelTextureApplier->GetTexturedVertices(VoxelPair.second);
  
        for (SnazzCraft::Vertice3D& Vertice3D : Vertices) { 
            Vertice3D.Position += Offset; // Adjusting to world space once now means not having to create a new model matrix for each individual chunk later

            NewTexturedVertices.push_back(Vertice3D);
            NewVerticesCount++;
        }
        
        for (unsigned int SideIndex = 0; SideIndex < 6; SideIndex++) {
            if (VoxelPair.second.Sides[SideIndex] == 0) continue;

            for (unsigned int I = 0; I < 6; I++) {
                NewIndices.push_back(SnazzCraft::VoxelMesh->Indices[(SideIndex * 6) + I] + NewTexturedVertices.size() - NewVerticesCount);
            }
        }
    }

    if (NewTexturedVertices.empty() || NewIndices.empty()) { this->ChunkMesh = nullptr; return; }

    this->ChunkMesh = new SnazzCraft::Mesh(NewTexturedVertices, NewIndices);
}

void SnazzCraft::Chunk::CullVoxelFaces()
{
    this->OptimizedVoxels->clear();

    for (auto VoxelPair : *this->Voxels)  {
        for (int I = 5; I >= 0; I--) {
            int CheckPosition[3] = {
                (int)(VoxelPair.second.Position[0]) + SnazzCraft::VoxelCheckPositions[I][0],
                (int)(VoxelPair.second.Position[1]) + SnazzCraft::VoxelCheckPositions[I][1],
                (int)(VoxelPair.second.Position[2]) + SnazzCraft::VoxelCheckPositions[I][2]
            };

            if (!VALID_VOXEL_POSITION(CheckPosition[0], CheckPosition[1], CheckPosition[2])) continue;

            auto CurrentIterator = this->Voxels->find(VOXEL_INDEX(CheckPosition[0], CheckPosition[1], CheckPosition[2]));
            if (CurrentIterator == this->Voxels->end()) continue;
     
            VoxelPair.second.Sides[I] = 0;
        }

        if (VoxelPair.second.GetSideCount() != 0) this->OptimizedVoxels->insert({ VoxelPair.first, VoxelPair.second });
    }
}

bool SnazzCraft::Chunk::VoxelTouchingChunkBorder(unsigned int VoxelIndex, unsigned int* BorderDirection)
{
    auto VoxelIterator = this->Voxels->find(VoxelIndex);
    if (VoxelIterator == this->Voxels->end()) return false;

    for (unsigned int I = 0; I < 6; I++) {
        int CheckX = static_cast<int>(VoxelIterator->second.Position[0]) + SnazzCraft::VoxelCheckPositions[I][0];
        int CheckY = static_cast<int>(VoxelIterator->second.Position[1]) + SnazzCraft::VoxelCheckPositions[I][1];
        int CheckZ = static_cast<int>(VoxelIterator->second.Position[2]) + SnazzCraft::VoxelCheckPositions[I][2];

        if (!VALID_VOXEL_POSITION(CheckX, CheckY, CheckZ)) {
            if (BorderDirection != nullptr) *BorderDirection = I;

            return true;
        }
    }

    return false;
}

bool SnazzCraft::Chunk::IsCollidingVoxel(const SnazzCraft::Hitbox& Hitbox)
{
    for (const auto& VoxelPair : *this->OptimizedVoxels) {
        glm::vec3 VoxelPosition = this->ChunkWorldOffset + glm::vec3(VoxelPair.second.Position[0], VoxelPair.second.Position[1], VoxelPair.second.Position[2]) * float(SnazzCraft::Voxel::Size) * glm::vec3(SnazzCraft::Chunk::Width, SnazzCraft::Chunk::Height, SnazzCraft::Chunk::Depth);

        //glm::vec3 voxelCenter = voxelMin + glm::vec3(SnazzCraft::Voxel::Size * 0.5f);

        if (Hitbox.IsCollidingVoxel(VoxelPosition)) return true;
    }

    return false;
}

