#include "snazzcraft-engine/world/chunk.hpp"

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

    this->VoxelCollisionHitbox = new SnazzCraft::Hitbox(glm::vec3(0.0f), glm::vec3((float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size));
}

SnazzCraft::Chunk::~Chunk()
{
    delete this->Voxels;
    delete this->OptimizedVoxels;
    delete this->LightValues;

    delete this->ChunkMesh;
    delete this->VoxelCollisionHitbox;
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

            this->Voxels->insert({ LOCAL_VOXEL_INDEX(X, Y, Z), SnazzCraft::Voxel(X, Y, Z, NewVoxelID) });
        }

        // Testing Torches
        if (X != 5 || Z != 5) continue;
        SnazzCraft::Voxel NewVoxel = SnazzCraft::Voxel(X, HeightAtPositionIterator->second, Z, ID_VOXEL_TORCH, false, false);
        NewVoxel.LightProducingLevel = 18;

        this->Voxels->insert({ LOCAL_VOXEL_INDEX(X, HeightAtPositionIterator->second, Z), NewVoxel });
    }
    }
}

void SnazzCraft::Chunk::UpdateMesh()
{
    delete this->ChunkMesh;

    std::vector<SnazzCraft::Vertice3D> NewTexturedVertices;
    std::vector<unsigned int> NewIndices;

    for (const auto& VoxelPair : *this->OptimizedVoxels) {
        const glm::vec3 Offset = glm::vec3((float)VoxelPair.second.Position[0], (float)VoxelPair.second.Position[1], (float)VoxelPair.second.Position[2]) * glm::vec3((float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size) + this->ChunkWorldOffset; 
        unsigned int NewVerticesCount = 0;

        std::vector<SnazzCraft::Vertice3D> Vertices = SnazzCraft::EngineVoxelTextureApplier->GetTexturedVertices(VoxelPair.second);
        this->ApplyBrightnessToVertices(Vertices, VoxelPair.second);
  
        for (SnazzCraft::Vertice3D& Vertice3D : Vertices) { 
            Vertice3D.Position += Offset; // Adjusting to world space once now means not having to create a new model matrix for each individual chunk later

            NewTexturedVertices.push_back(Vertice3D);
            NewVerticesCount++; 
        }
        
        for (unsigned int SideIndex = 0; SideIndex < 6; SideIndex++) {
            if (!VoxelPair.second.Sides[SideIndex]) continue;

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
        if (!VoxelPair.second.Cullable) { this->OptimizedVoxels->insert({ VoxelPair.first, VoxelPair.second }); continue; }

        for (int I = 5; I >= 0; I--) {
            int CheckPosition[3] = {
                (int)(VoxelPair.second.Position[0]) + SnazzCraft::VoxelCheckPositions[I][0],
                (int)(VoxelPair.second.Position[1]) + SnazzCraft::VoxelCheckPositions[I][1],
                (int)(VoxelPair.second.Position[2]) + SnazzCraft::VoxelCheckPositions[I][2]
            };

            if (!VALID_LOCAL_VOXEL_POSITION(CheckPosition[0], CheckPosition[1], CheckPosition[2])) continue;

            auto CurrentIterator = this->Voxels->find(LOCAL_VOXEL_INDEX(CheckPosition[0], CheckPosition[1], CheckPosition[2]));
            if (CurrentIterator == this->Voxels->end()) continue;

            if (!CurrentIterator->second.Cullable) continue;
      
            VoxelPair.second.Sides[I] = false;
        }

        if (VoxelPair.second.GetSideCount() != 0) this->OptimizedVoxels->insert({ VoxelPair.first, VoxelPair.second });
    }
}

bool SnazzCraft::Chunk::VoxelTouchingChunkBorder(unsigned int VoxelIndex, unsigned int* BorderDirection) const
{
    auto VoxelIterator = this->Voxels->find(VoxelIndex);
    if (VoxelIterator == this->Voxels->end()) return false;

    for (unsigned int I = 0; I < 6; I++) {
        int CheckX = static_cast<int>(VoxelIterator->second.Position[0]) + SnazzCraft::VoxelCheckPositions[I][0];
        int CheckY = static_cast<int>(VoxelIterator->second.Position[1]) + SnazzCraft::VoxelCheckPositions[I][1];
        int CheckZ = static_cast<int>(VoxelIterator->second.Position[2]) + SnazzCraft::VoxelCheckPositions[I][2];

        if (!VALID_LOCAL_VOXEL_POSITION(CheckX, CheckY, CheckZ)) {
            if (BorderDirection != nullptr) *BorderDirection = I;

            return true;
        }
    }

    return false;
}

SnazzCraft::Voxel* SnazzCraft::Chunk::GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox) const
{
    int Range[3] = {
        static_cast<int>(glm::ceil(Hitbox->HalfDimensions[0])),
        static_cast<int>(glm::ceil(Hitbox->HalfDimensions[1])),
        static_cast<int>(glm::ceil(Hitbox->HalfDimensions[2]))
    };

    int VPosition[3];
    this->WorldSpaceToVoxelSpace(Hitbox->Position, VPosition);

    for (int X = VPosition[0] - Range[0]; X <= VPosition[0] + Range[0]; X++) {
    for (int Y = VPosition[1] - Range[1]; Y <= VPosition[1] + Range[1]; Y++) {
    for (int Z = VPosition[2] - Range[2]; Z <= VPosition[2] + Range[2]; Z++) {
        SnazzCraft::Voxel* CollidingVoxel = this->GetCollidingVoxel(Hitbox, X, Y, Z);
        if (CollidingVoxel != nullptr) return CollidingVoxel;
    }
    }
    }

    return nullptr;
}

SnazzCraft::Voxel* SnazzCraft::Chunk::GetCollidingVoxel(const glm::vec3& Position) const
{
    int VPosition[3];
    glm::vec3 CheckPosition = Position;
    this->WorldSpaceToVoxelSpace(CheckPosition, VPosition);

    if (!VALID_LOCAL_VOXEL_POSITION(VPosition[0], VPosition[1], VPosition[2])) return nullptr;

    auto VoxelIterator = this->Voxels->find(LOCAL_VOXEL_INDEX(VPosition[0], VPosition[1], VPosition[2]));
    if (VoxelIterator == this->Voxels->end()) return nullptr;

    if (!VoxelIterator->second.Collidable) return nullptr;

    this->VoxelCollisionHitbox->Position = this->LocalVoxelPositionToWorldPosition(VPosition[0], VPosition[1], VPosition[2]); 
    if (this->VoxelCollisionHitbox->IsColliding(CheckPosition)) return &VoxelIterator->second;

    return nullptr;
}

SnazzCraft::Voxel* SnazzCraft::Chunk::GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox, int LocalVoxelX, int LocalVoxelY, int LocalVoxelZ) const
{
    if (!VALID_LOCAL_VOXEL_POSITION(LocalVoxelX, LocalVoxelY, LocalVoxelZ)) return nullptr;

    auto VoxelIterator = this->Voxels->find(LOCAL_VOXEL_INDEX(LocalVoxelX, LocalVoxelY, LocalVoxelZ));
    if (VoxelIterator == this->Voxels->end()) return nullptr;

    if (!VoxelIterator->second.Collidable) return nullptr;

    this->VoxelCollisionHitbox->Position = this->LocalVoxelPositionToWorldPosition(LocalVoxelX, LocalVoxelY, LocalVoxelZ); 
    if (!this->VoxelCollisionHitbox->IsColliding(*Hitbox)) return nullptr;

    return &VoxelIterator->second;
}

void SnazzCraft::Chunk::ApplyBrightnessToVertices(std::vector<SnazzCraft::Vertice3D>& Vertices, const SnazzCraft::Voxel& Voxel) const
{
    int LightApplyValue = 1;

    auto LightIterator = this->LightValues->find(INDEX_3D(Voxel.Position[0], Voxel.Position[1], Voxel.Position[2], SnazzCraft::Chunk::Width, SnazzCraft::Chunk::Height));
    if (LightIterator != this->LightValues->end()) {
        if (LightIterator->second > LightApplyValue) LightApplyValue = LightIterator->second;
    }

    if (Voxel.LightProducingLevel > LightApplyValue) LightApplyValue = Voxel.LightProducingLevel;

    for (SnazzCraft::Vertice3D& Vertice : Vertices) {
        Vertice.Brightness = LightApplyValue / static_cast<float>(MAX_BRIGHTNESS);
    }
}

