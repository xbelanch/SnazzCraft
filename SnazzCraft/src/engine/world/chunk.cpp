#include "snazzcraft-engine/world/chunk.hpp"

SnazzCraft::Chunk::Chunk(int32_t X, int32_t Y)
: ChunkMesh(nullptr), Voxels(std::unordered_map<uint32_t, SnazzCraft::Voxel>()), 
  LightValues(std::unordered_map<uint32_t, int>()), Vertices(std::vector<SnazzCraft::VoxelVertice>()), Indices(std::vector<uint32_t>()), 
  VoxelCollisionHitbox(new SnazzCraft::Hitbox(glm::vec3((float)SnazzCraft::Voxel::Size)))
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
    delete this->ChunkMesh;
    delete this->VoxelCollisionHitbox;
}

void SnazzCraft::Chunk::Generate(SnazzCraft::HeightMap* HeightMap, uint32_t MapWidth)
{
    uint32_t HeightMapOffsetX = this->Position[0] * SnazzCraft::Chunk::Width;
    uint32_t HeightMapOffsetZ = this->Position[1] * SnazzCraft::Chunk::Depth;

    for (uint32_t X = 0; X < SnazzCraft::Chunk::Width; X++) {
    for (uint32_t Z = 0; Z < SnazzCraft::Chunk::Depth; Z++) {
        HeightMap->GenerateValue(X + HeightMapOffsetX, Z + HeightMapOffsetZ);
        auto HeightAtPositionIterator = HeightMap->HeightValues.find((Z + HeightMapOffsetZ) * MapWidth + (X + HeightMapOffsetX));
        if (HeightAtPositionIterator == HeightMap->HeightValues.end()) continue;

        uint32_t HeightToGenerate = HeightAtPositionIterator->second + SnazzCraft::Chunk::OceanLevel;
        uint32_t Y = 0;
        while (true)
        {
            uint32_t NewVoxelID = ID_VOXEL_STONE;

            if (HeightToGenerate != 0 && Y == HeightToGenerate - 1) {
                if (Y >= SnazzCraft::Chunk::OceanLevel - 1) {
                    NewVoxelID = ID_VOXEL_DIRT_GRASS_MIX;
                } else {
                    NewVoxelID = ID_VOXEL_DIRT; 
                }
            } else if (HeightToGenerate != 0 && Y >= HeightToGenerate - 4) {
                NewVoxelID = ID_VOXEL_DIRT;
            }

            SnazzCraft::Voxel NewVoxel(X, Y, Z, NewVoxelID);
            this->Voxels.insert({ SnazzCraft::Chunk::LocalVoxelIndex(X, Y, Z), NewVoxel });

            Y++;
            if (Y >= HeightToGenerate) {
                if (Y >= SnazzCraft::Chunk::OceanLevel) break;
                
                while (Y < SnazzCraft::Chunk::OceanLevel)
                {
                    SnazzCraft::Voxel NewVoxel(X, Y, Z, ID_VOXEL_WATER);
                    this->Voxels.insert({ SnazzCraft::Chunk::LocalVoxelIndex(X, Y, Z), NewVoxel });
                    Y++;
                }
                
                break;
            }
        }

        // Testing Torches
        if (X != 5 || Z != 5 || HeightToGenerate <= SnazzCraft::Chunk::OceanLevel) continue;
        SnazzCraft::Voxel NewVoxel = SnazzCraft::Voxel(X, HeightToGenerate, Z, ID_VOXEL_TORCH);

        this->Voxels.insert({ SnazzCraft::Chunk::LocalVoxelIndex(X, HeightToGenerate, Z), NewVoxel });
    }
    }
}

void SnazzCraft::Chunk::UpdateVerticesAndIndices()
{
    this->Vertices.clear();
    this->Indices.clear();

    for (const auto& VoxelPair : this->Voxels) {
        if (VoxelPair.second.GetSideCount() == 0) continue;

        const glm::vec3 Offset = glm::vec3((float)VoxelPair.second.X, (float)VoxelPair.second.Y, (float)VoxelPair.second.Z) * glm::vec3((float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size, (float)SnazzCraft::Voxel::Size) + this->ChunkWorldOffset; 
        uint32_t NewVerticesCount = 0;

        for (SnazzCraft::VoxelVertice& VoxelVertice : SnazzCraft::EngineVoxelTextureApplier->GetTexturedVertices(VoxelPair.second)) { 
            VoxelVertice.Position += Offset; // Adjusting to world space once now means not having to create a new model matrix for each individual chunk later

            this->Vertices.push_back(VoxelVertice);
            NewVerticesCount++; 
        }
        
        for (uint32_t SideIndex = 0; SideIndex < 6; SideIndex++) {
            if (!VoxelPair.second.HasSide(SideIndex)) continue;

            for (uint32_t I = 0; I < 6; I++) {
                this->Indices.push_back(SnazzCraft::VoxelMesh->Indices[INDEX_2D(I, SideIndex, 6)] + this->Vertices.size() - NewVerticesCount);
            }
            
        }
    }
}

void SnazzCraft::Chunk::CullVoxelFaces()
{
    for (auto& VoxelPair : this->Voxels)  {
        const SnazzCraft::VoxelType& PairType = VoxelPair.second.GetVoxelType();

        for (int8_t I = 5; I >= 0; I--) {
            if (!SnazzCraft::AccessBitValue(PairType.CullableSides, I)) continue;

            int32_t CheckPosition[3] = {
                (int)(VoxelPair.second.X) + SnazzCraft::VoxelCheckPositions[I][0],
                (int)(VoxelPair.second.Y) + SnazzCraft::VoxelCheckPositions[I][1],
                (int)(VoxelPair.second.Z) + SnazzCraft::VoxelCheckPositions[I][2]
            };

            if (!SnazzCraft::Chunk::WithinChunkBounds(CheckPosition[0], CheckPosition[1], CheckPosition[2])) continue;

            auto CurrentIterator = this->Voxels.find(SnazzCraft::Chunk::LocalVoxelIndex(CheckPosition[0], CheckPosition[1], CheckPosition[2]));
            if (CurrentIterator == this->Voxels.end()) continue;

            if (!SnazzCraft::AccessBitValue(CurrentIterator->second.GetVoxelType().CullableSides, I)) continue;
      
            VoxelPair.second.ChangeSideValue(I, false);
        }
    }
}

bool SnazzCraft::Chunk::VoxelTouchingChunkBorder(uint32_t VoxelIndex, uint32_t* BorderDirection) const
{
    auto VoxelIterator = this->Voxels.find(VoxelIndex);
    if (VoxelIterator == this->Voxels.end()) return false;

    for (uint32_t I = 0; I < 6; I++) {
        int32_t CheckX = static_cast<int>(VoxelIterator->second.X) + SnazzCraft::VoxelCheckPositions[I][0];
        int32_t CheckY = static_cast<int>(VoxelIterator->second.Y) + SnazzCraft::VoxelCheckPositions[I][1];
        int32_t CheckZ = static_cast<int>(VoxelIterator->second.Z) + SnazzCraft::VoxelCheckPositions[I][2];

        if (!SnazzCraft::Chunk::WithinChunkBounds(CheckX, CheckY, CheckZ)) {
            if (BorderDirection != nullptr) *BorderDirection = I;

            return true;
        }
    }

    return false;
}

SnazzCraft::Voxel* SnazzCraft::Chunk::GetCollidingVoxel(const glm::vec3& Position, const SnazzCraft::Hitbox* Hitbox)
{
    int32_t Range[3] = {
        static_cast<int>(glm::ceil(Hitbox->HalfDimensions[0])),
        static_cast<int>(glm::ceil(Hitbox->HalfDimensions[1])),
        static_cast<int>(glm::ceil(Hitbox->HalfDimensions[2]))
    };

    int32_t VPosition[3];
    this->WorldSpaceToVoxelSpace(Position, VPosition);

    for (int32_t X = VPosition[0] - Range[0]; X <= VPosition[0] + Range[0]; X++) {
    for (int32_t Y = VPosition[1] - Range[1]; Y <= VPosition[1] + Range[1]; Y++) {
    for (int32_t Z = VPosition[2] - Range[2]; Z <= VPosition[2] + Range[2]; Z++) {
        SnazzCraft::Voxel* CollidingVoxel = this->GetCollidingVoxel(Position, Hitbox, X, Y, Z);
        if (CollidingVoxel != nullptr) return CollidingVoxel;
    }
    }
    }

    return nullptr;
}

SnazzCraft::Voxel* SnazzCraft::Chunk::GetCollidingVoxel(const glm::vec3& Position)
{
    int32_t VPosition[3];
    glm::vec3 CheckPosition = Position;
    this->WorldSpaceToVoxelSpace(CheckPosition, VPosition);

    if (!SnazzCraft::Chunk::WithinChunkBounds(VPosition[0], VPosition[1], VPosition[2])) return nullptr;

    auto VoxelIterator = this->Voxels.find(SnazzCraft::Chunk::LocalVoxelIndex(VPosition[0], VPosition[1], VPosition[2]));
    if (VoxelIterator == this->Voxels.end()) return nullptr;

    if (!VoxelIterator->second.GetVoxelType().CollidableToEntities) return nullptr;

    if (this->VoxelCollisionHitbox->IsColliding(this->LocalVoxelPositionToWorldPosition(VPosition[0], VPosition[1], VPosition[2]), CheckPosition)) return &VoxelIterator->second;

    return nullptr;
}

SnazzCraft::Voxel* SnazzCraft::Chunk::GetCollidingVoxel(const glm::vec3& Position, const SnazzCraft::Hitbox* Hitbox, int32_t LocalVoxelX, int32_t LocalVoxelY, int32_t LocalVoxelZ)
{
    if (!SnazzCraft::Chunk::WithinChunkBounds(LocalVoxelX, LocalVoxelY, LocalVoxelZ)) return nullptr;

    auto VoxelIterator = this->Voxels.find(SnazzCraft::Chunk::LocalVoxelIndex(LocalVoxelX, LocalVoxelY, LocalVoxelZ));
    if (VoxelIterator == this->Voxels.end()) return nullptr;

    if (!VoxelIterator->second.GetVoxelType().CollidableToEntities) return nullptr;

    if (!this->VoxelCollisionHitbox->IsColliding(this->LocalVoxelPositionToWorldPosition(LocalVoxelX, LocalVoxelY, LocalVoxelZ), *Hitbox, Position)) return nullptr;

    return &VoxelIterator->second;
}

void SnazzCraft::Chunk::UpdateLightingOnVertices(const std::unordered_map<uint32_t, SnazzCraft::Chunk*>& Chunks, uint32_t WorldSize)
{
    auto GetLightValue = [this, Chunks, WorldSize](const SnazzCraft::Voxel& Voxel, int8_t Side) -> float
    {
        constexpr float DefaultLightValue = 1.0f / static_cast<float>(SnazzCraft::Voxel::MaxLightValue);

        auto LightValueIterator = this->LightValues.find(SnazzCraft::Chunk::LocalVoxelIndex(Voxel));
        if (LightValueIterator == this->LightValues.end()) return DefaultLightValue;

        return static_cast<float>(LightValueIterator->second) / SnazzCraft::Voxel::MaxLightValue;
    };

    uint32_t VoxelCount = 0;
    for (const auto& [Key, Voxel] : this->Voxels) {
        if (Voxel.GetSideCount() == 0) continue;

        uint32_t VerticeIndex = VoxelCount * 24; // 24 vertices per voxel
        for (uint32_t L = 0; L < 6; L++) { // 6 faces per voxel
            if (!Voxel.HasSide(L)) continue;

            float LightValue = GetLightValue(Voxel, L);
            for (uint32_t J = 0; J < 4; J++) { // 4 vertices per face
                this->Vertices[VerticeIndex + INDEX_2D(J, L, 4)].Brightness = LightValue;
            }
        }

        VoxelCount++;
    }
}

