#include "snazzcraft-engine/world/chunk.hpp"

SnazzCraft::Chunk::Chunk(int32_t X, int32_t Y)
{
    this->Position[0] = X;
    this->Position[1] = Y;

    this->ChunkWorldOffset = {
        this->Position[0] * SnazzCraft::Chunk::Width * SnazzCraft::Voxel::Size,
        0.0f,
        this->Position[1] * SnazzCraft::Chunk::Depth * SnazzCraft::Voxel::Size 
    };

    this->VoxelCollisionHitbox = new SnazzCraft::Hitbox(glm::vec3(0.0f), glm::vec3((float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size));

    this->Vertices = new std::vector<SnazzCraft::Vertice3D>();
    this->Indices = new std::vector<uint32_t>();
}

SnazzCraft::Chunk::~Chunk()
{
    delete this->Voxels;
    delete this->OptimizedVoxels;
    delete this->LightValues;

    delete this->ChunkMesh;
    delete this->VoxelCollisionHitbox;

    delete this->Vertices;
    delete this->Indices;
}

void SnazzCraft::Chunk::Generate(SnazzCraft::HeightMap* HeightMap, uint32_t MapWidth)
{
    uint32_t HeightMapOffsetX = this->Position[0] * SnazzCraft::Chunk::Width;
    uint32_t HeightMapOffsetZ = this->Position[1] * SnazzCraft::Chunk::Depth;

    for (uint32_t X = 0; X < SnazzCraft::Chunk::Width; X++) {
    for (uint32_t Z = 0; Z < SnazzCraft::Chunk::Depth; Z++) {
        HeightMap->GenerateValue(X + HeightMapOffsetX, Z + HeightMapOffsetZ);
        auto HeightAtPositionIterator = HeightMap->HeightValues->find((Z + HeightMapOffsetZ) * MapWidth + (X + HeightMapOffsetX));

        for (uint32_t Y = 0; Y < HeightAtPositionIterator->second; Y++) {
            uint32_t NewVoxelID = ID_VOXEL_STONE;

            if (HeightAtPositionIterator->second != 0 && Y == HeightAtPositionIterator->second - 1) NewVoxelID = ID_VOXEL_DIRT_GRASS_MIX;
            else if (HeightAtPositionIterator->second != 0 && Y >= HeightAtPositionIterator->second - 4) NewVoxelID = ID_VOXEL_DIRT;

            this->Voxels->insert({ SnazzCraft::Chunk::LocalVoxelIndex(X, Y, Z), SnazzCraft::Voxel(X, Y, Z, NewVoxelID) });
        }

        // Testing Torches
        if (X != 5 || Z != 5) continue;
        SnazzCraft::Voxel NewVoxel = SnazzCraft::Voxel(X, HeightAtPositionIterator->second, Z, ID_VOXEL_TORCH, false, false);
        NewVoxel.LightProducingLevel = 18;

        this->Voxels->insert({ SnazzCraft::Chunk::LocalVoxelIndex(X, HeightAtPositionIterator->second, Z), NewVoxel });
    }
    }
}

void SnazzCraft::Chunk::UpdateVerticesAndIndices()
{


    this->Vertices->clear();
    this->Indices->clear();

    for (const auto& VoxelPair : *this->OptimizedVoxels) {
        const glm::vec3 Offset = glm::vec3((float)VoxelPair.second.Position[0], (float)VoxelPair.second.Position[1], (float)VoxelPair.second.Position[2]) * glm::vec3((float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size) + this->ChunkWorldOffset; 
        uint32_t NewVerticesCount = 0;

        for (SnazzCraft::Vertice3D& Vertice3D : SnazzCraft::EngineVoxelTextureApplier->GetTexturedVertices(VoxelPair.second)) { 
            Vertice3D.Position += Offset; // Adjusting to world space once now means not having to create a new model matrix for each individual chunk later

            this->Vertices->push_back(Vertice3D);
            NewVerticesCount++; 
        }
        
        for (uint32_t SideIndex = 0; SideIndex < 6; SideIndex++) {
            if (!VoxelPair.second.Sides[SideIndex]) continue;

            for (uint32_t I = 0; I < 6; I++) {
                this->Indices->push_back(SnazzCraft::VoxelMesh->Indices[INDEX_2D(I, SideIndex, 6)] + this->Vertices->size() - NewVerticesCount);
            }
        }
    }
}

void SnazzCraft::Chunk::CullVoxelFaces()
{
    this->OptimizedVoxels->clear();

    for (auto VoxelPair : *this->Voxels)  {
        if (!VoxelPair.second.Cullable) { this->OptimizedVoxels->insert({ VoxelPair.first, VoxelPair.second }); continue; }

        for (int32_t I = 5; I >= 0; I--) {
            int32_t CheckPosition[3] = {
                (int)(VoxelPair.second.Position[0]) + SnazzCraft::VoxelCheckPositions[I][0],
                (int)(VoxelPair.second.Position[1]) + SnazzCraft::VoxelCheckPositions[I][1],
                (int)(VoxelPair.second.Position[2]) + SnazzCraft::VoxelCheckPositions[I][2]
            };

            if (!SnazzCraft::Chunk::ValidLocalVoxelPosition(CheckPosition[0], CheckPosition[1], CheckPosition[2])) continue;

            auto CurrentIterator = this->Voxels->find(SnazzCraft::Chunk::LocalVoxelIndex(CheckPosition[0], CheckPosition[1], CheckPosition[2]));
            if (CurrentIterator == this->Voxels->end()) continue;

            if (!CurrentIterator->second.Cullable) continue;
      
            VoxelPair.second.Sides[I] = false;
        }

        if (VoxelPair.second.GetSideCount() != 0) this->OptimizedVoxels->insert({ VoxelPair.first, VoxelPair.second });
    }
}

bool SnazzCraft::Chunk::VoxelTouchingChunkBorder(uint32_t VoxelIndex, uint32_t* BorderDirection) const
{
    auto VoxelIterator = this->Voxels->find(VoxelIndex);
    if (VoxelIterator == this->Voxels->end()) return false;

    for (uint32_t I = 0; I < 6; I++) {
        int32_t CheckX = static_cast<int>(VoxelIterator->second.Position[0]) + SnazzCraft::VoxelCheckPositions[I][0];
        int32_t CheckY = static_cast<int>(VoxelIterator->second.Position[1]) + SnazzCraft::VoxelCheckPositions[I][1];
        int32_t CheckZ = static_cast<int>(VoxelIterator->second.Position[2]) + SnazzCraft::VoxelCheckPositions[I][2];

        if (!SnazzCraft::Chunk::ValidLocalVoxelPosition(CheckX, CheckY, CheckZ)) {
            if (BorderDirection != nullptr) *BorderDirection = I;

            return true;
        }
    }

    return false;
}

SnazzCraft::Voxel* SnazzCraft::Chunk::GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox) const
{
    int32_t Range[3] = {
        static_cast<int>(glm::ceil(Hitbox->HalfDimensions[0])),
        static_cast<int>(glm::ceil(Hitbox->HalfDimensions[1])),
        static_cast<int>(glm::ceil(Hitbox->HalfDimensions[2]))
    };

    int32_t VPosition[3];
    this->WorldSpaceToVoxelSpace(Hitbox->Position, VPosition);

    for (int32_t X = VPosition[0] - Range[0]; X <= VPosition[0] + Range[0]; X++) {
    for (int32_t Y = VPosition[1] - Range[1]; Y <= VPosition[1] + Range[1]; Y++) {
    for (int32_t Z = VPosition[2] - Range[2]; Z <= VPosition[2] + Range[2]; Z++) {
        SnazzCraft::Voxel* CollidingVoxel = this->GetCollidingVoxel(Hitbox, X, Y, Z);
        if (CollidingVoxel != nullptr) return CollidingVoxel;
    }
    }
    }

    return nullptr;
}

SnazzCraft::Voxel* SnazzCraft::Chunk::GetCollidingVoxel(const glm::vec3& Position) const
{
    int32_t VPosition[3];
    glm::vec3 CheckPosition = Position;
    this->WorldSpaceToVoxelSpace(CheckPosition, VPosition);

    if (!SnazzCraft::Chunk::ValidLocalVoxelPosition(VPosition[0], VPosition[1], VPosition[2])) return nullptr;

    auto VoxelIterator = this->Voxels->find(SnazzCraft::Chunk::LocalVoxelIndex(VPosition[0], VPosition[1], VPosition[2]));
    if (VoxelIterator == this->Voxels->end()) return nullptr;

    if (!VoxelIterator->second.Collidable) return nullptr;

    this->VoxelCollisionHitbox->Position = this->LocalVoxelPositionToWorldPosition(VPosition[0], VPosition[1], VPosition[2]); 
    if (this->VoxelCollisionHitbox->IsColliding(CheckPosition)) return &VoxelIterator->second;

    return nullptr;
}

SnazzCraft::Voxel* SnazzCraft::Chunk::GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox, int32_t LocalVoxelX, int32_t LocalVoxelY, int32_t LocalVoxelZ) const
{
    if (!SnazzCraft::Chunk::ValidLocalVoxelPosition(LocalVoxelX, LocalVoxelY, LocalVoxelZ)) return nullptr;

    auto VoxelIterator = this->Voxels->find(SnazzCraft::Chunk::LocalVoxelIndex(LocalVoxelX, LocalVoxelY, LocalVoxelZ));
    if (VoxelIterator == this->Voxels->end()) return nullptr;

    if (!VoxelIterator->second.Collidable) return nullptr;

    this->VoxelCollisionHitbox->Position = this->LocalVoxelPositionToWorldPosition(LocalVoxelX, LocalVoxelY, LocalVoxelZ); 
    if (!this->VoxelCollisionHitbox->IsColliding(*Hitbox)) return nullptr;

    return &VoxelIterator->second;
}



