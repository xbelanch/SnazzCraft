#include"snazzcraft-engine/world/world.hpp"

bool GetNewPlacePosition(const glm::vec3& EndPosition, uint8_t FaceHit, SnazzCraft::Voxel* VoxelHit, int8_t OutNewPlacePosition[3], int32_t OutChunkCoordinates[2]);

bool SnazzCraft::World::DestroyVoxel(const glm::vec3& Position, const glm::vec3& Rotation)
{
    return false;
}

bool SnazzCraft::World::PlaceVoxel(const glm::vec3& Position, const glm::vec3& Rotation, uint8_t VoxelID)
{
    auto CollidedWithEntity = [this](uint32_t NewlyPlacedVoxelIndex) -> bool
    {
        SnazzCraft::Voxel* CollidingVoxel = this->GetCollidingVoxel(SnazzCraft::Player->Position, SnazzCraft::Player->GetEntityType().EntityHitbox);
            if (CollidingVoxel != nullptr && SnazzCraft::Chunk::LocalVoxelIndex(*CollidingVoxel) == NewlyPlacedVoxelIndex) return true;

        for (SnazzCraft::Entity* Entity : this->Entities) {
            SnazzCraft::Voxel* CollidingVoxel = this->GetCollidingVoxel(Entity->Position, Entity->GetEntityType().EntityHitbox);
            if (CollidingVoxel != nullptr && SnazzCraft::Chunk::LocalVoxelIndex(*CollidingVoxel) == NewlyPlacedVoxelIndex) return true;
        }

        return false;
    };

    glm::vec3 EndPosition = Position;
    uint8_t FaceHit;
    SnazzCraft::Voxel* VoxelHit;

    this->RaycastToVoxel(EndPosition, Rotation, this->PlayerReach, &FaceHit, &VoxelHit);
    if (VoxelHit == nullptr || EndPosition == Position) return false;

    int8_t NewPlacePosition[3];
    int32_t ChunkCoordinates[2];
    if (!GetNewPlacePosition(EndPosition, FaceHit, VoxelHit, NewPlacePosition, ChunkCoordinates)) return false;
    
    auto ChunkIterator = this->Chunks.find(INDEX_2D(ChunkCoordinates[0], ChunkCoordinates[1], this->Size));
    if (ChunkIterator == this->Chunks.end() || ChunkCoordinates[0] < 0 || ChunkCoordinates[1] < 0 || ChunkCoordinates[0] >= static_cast<int32_t>(this->Size) || ChunkCoordinates[1] >= static_cast<int32_t>(this->Size)) return false;
    
    uint32_t LocalPlaceVoxelIndex = SnazzCraft::Chunk::LocalVoxelIndex(NewPlacePosition[0], NewPlacePosition[1], NewPlacePosition[2]);
    ChunkIterator->second->Voxels.insert_or_assign(LocalPlaceVoxelIndex, SnazzCraft::Voxel(static_cast<uint8_t>(NewPlacePosition[0]), static_cast<uint8_t>(NewPlacePosition[1]), static_cast<uint8_t>(NewPlacePosition[2]), VoxelID));

    if (CollidedWithEntity(LocalPlaceVoxelIndex)) { ChunkIterator->second->Voxels.erase(LocalPlaceVoxelIndex); return false; }

    ChunkIterator->second->CullVoxelFaces();
    ChunkIterator->second->UpdateVerticesAndIndices();
    this->UpdateChunkLighting(ChunkIterator->second, nullptr);
    
    return true;
}

void SnazzCraft::World::UpdateVoxelPlacementDisplayPosition()
{
    glm::vec3 EndPosition = SnazzCraft::Player->Position;
    uint8_t FaceHit;
    SnazzCraft::Voxel* VoxelHit = nullptr;

    this->RaycastToVoxel(EndPosition, SnazzCraft::Player->Rotation, this->PlayerReach, &FaceHit, &VoxelHit);
    if (VoxelHit == nullptr) { this->RenderVoxelPlacementDisplay = false; return; }

    int8_t NewPlacePosition[3];
    int32_t ChunkCoordinates[2];
    if (!GetNewPlacePosition(EndPosition, FaceHit, VoxelHit, NewPlacePosition, ChunkCoordinates)) { this->RenderVoxelPlacementDisplay = false; return; }

    auto ChunkIterator = this->Chunks.find(INDEX_2D(ChunkCoordinates[0], ChunkCoordinates[1], this->Size));
    if (ChunkIterator == this->Chunks.end() || ChunkCoordinates[0] < 0 || ChunkCoordinates[1] < 0 || ChunkCoordinates[0] >= static_cast<int32_t>(this->Size) || ChunkCoordinates[1] >= static_cast<int32_t>(this->Size)) { this->RenderVoxelPlacementDisplay = false; return; }

    this->VoxelPlacementDisplayPosition = glm::vec3(
        (static_cast<float>(ChunkIterator->second->Position[0] * SnazzCraft::Chunk::Width) + NewPlacePosition[0]) * SnazzCraft::Voxel::Size,
        (static_cast<float>(NewPlacePosition[1])) * SnazzCraft::Voxel::Size,
        (static_cast<float>(ChunkIterator->second->Position[1] * SnazzCraft::Chunk::Depth) + NewPlacePosition[2]) * SnazzCraft::Voxel::Size
    );

    this->RenderVoxelPlacementDisplay = true;
}

bool SnazzCraft::World::RaycastToVoxel(glm::vec3& Position, const glm::vec3& Rotation, float MaxDistance, uint8_t* FaceHit, SnazzCraft::Voxel** VoxelHit)
{
    auto UpdateFaceHitAndVoxelHit = [FaceHit, VoxelHit](const glm::ivec3& Step, const uint8_t& LastStepAxis, SnazzCraft::Voxel* HitVoxel) -> void
    {
        if (FaceHit == nullptr) goto UpdateHitVoxel; // A goto statement, how exciting!
        switch (LastStepAxis)
        {
            case 0x00:
                *FaceHit = Step.x == -1 ? 2 : 1;
                break;

            case 0x01:
                *FaceHit = Step.y == -1 ? 4 : 5;
                break;
            
            case 0x02:
                *FaceHit = Step.z == -1 ? 3 : 0;
                break;
        }

        UpdateHitVoxel:
        if (VoxelHit != nullptr) *VoxelHit = HitVoxel;
    };

    float VoxelSize = static_cast<float>(SnazzCraft::Voxel::Size);
    glm::vec3 RayOrigin = Position;

    glm::vec3 RayEnd = Position;
    SnazzCraft::MoveVector3D(RayEnd, Rotation, 1.0f);
    glm::vec3 RayDirection = glm::normalize(RayEnd - RayOrigin);

    glm::ivec3 MapPosition = glm::ivec3(glm::floor(RayOrigin / VoxelSize));

    glm::vec3 DeltaDistance = glm::vec3(
        std::abs(VoxelSize / RayDirection.x),
        std::abs(VoxelSize / RayDirection.y),
        std::abs(VoxelSize / RayDirection.z)
    );

    glm::ivec3 Step;
    glm::vec3 SideDistance;

    if (RayDirection.x < 0) {
        Step.x = -1;
        SideDistance.x = (RayOrigin.x - (float(MapPosition.x) * VoxelSize)) * (DeltaDistance.x / VoxelSize);
    } else {
        Step.x = 1;
        SideDistance.x = ((float(MapPosition.x) + 1.0f) * VoxelSize - RayOrigin.x) * (DeltaDistance.x / VoxelSize);
    }

    if (RayDirection.y < 0) {
        Step.y = -1;
        SideDistance.y = (RayOrigin.y - (float(MapPosition.y) * VoxelSize)) * (DeltaDistance.y / VoxelSize);
    } else {
        Step.y = 1;
        SideDistance.y = ((float(MapPosition.y) + 1.0f) * VoxelSize - RayOrigin.y) * (DeltaDistance.y / VoxelSize);
    }

    if (RayDirection.z < 0) {
        Step.z = -1;
        SideDistance.z = (RayOrigin.z - (float(MapPosition.z) * VoxelSize)) * (DeltaDistance.z / VoxelSize);
    } else {
        Step.z = 1;
        SideDistance.z = ((float(MapPosition.z) + 1.0f) * VoxelSize - RayOrigin.z) * (DeltaDistance.z / VoxelSize);
    }

    // 0 == X
    // 1 == Y
    // 2 == Z
    uint8_t LastStepAxis = 0x00; 
    float DistanceTraveled = 0.0f;
    while (DistanceTraveled < MaxDistance)
    {
        SnazzCraft::Voxel* HitVoxel = this->GetCollidingVoxel(glm::vec3(MapPosition) * VoxelSize);
        if (HitVoxel != nullptr) {
            Position = RayOrigin + (RayDirection * DistanceTraveled);
            UpdateFaceHitAndVoxelHit(Step, LastStepAxis, HitVoxel);
            return true;
        }

        if (SideDistance.x < SideDistance.y && SideDistance.x < SideDistance.z) {
            DistanceTraveled = SideDistance.x;
            SideDistance.x += DeltaDistance.x;
            MapPosition.x += Step.x;
            LastStepAxis = 0x00;
        } 
        else if (SideDistance.y < SideDistance.z) {
            DistanceTraveled = SideDistance.y;
            SideDistance.y += DeltaDistance.y;
            MapPosition.y += Step.y;
            LastStepAxis = 0x01;
        } 
        else {
            DistanceTraveled = SideDistance.z;
            SideDistance.z += DeltaDistance.z;
            MapPosition.z += Step.z;
            LastStepAxis = 0x02;
        }
    }

    UpdateFaceHitAndVoxelHit(Step, LastStepAxis, nullptr);
    Position = RayOrigin + (RayDirection * MaxDistance);
    return false;
}

bool GetNewPlacePosition(const glm::vec3& EndPosition, uint8_t FaceHit, SnazzCraft::Voxel* VoxelHit, int8_t OutNewPlacePosition[3], int32_t OutChunkCoordinates[2])
{
    glm::ivec3 VoxelSpacePosition = EndPosition / glm::vec3(static_cast<float>(SnazzCraft::Voxel::Size));
    SnazzCraft::Chunk::GetChunkPosition(VoxelSpacePosition.x, VoxelSpacePosition.z, OutChunkCoordinates);

    OutNewPlacePosition[0] = VoxelHit->X;
    OutNewPlacePosition[1] = VoxelHit->Y;
    OutNewPlacePosition[2] = VoxelHit->Z;
    switch (FaceHit) // Front -> Bottom
    {
        case 0: 
            OutNewPlacePosition[2]--;
            if (OutNewPlacePosition[2] < 0) { 
                OutNewPlacePosition[2] += SnazzCraft::Chunk::Depth;
                OutChunkCoordinates[1]--;
            }
            break;
        case 1:
            OutNewPlacePosition[0]--;
            if (OutNewPlacePosition[0] < 0) { 
                OutNewPlacePosition[0] += SnazzCraft::Chunk::Width;
                OutChunkCoordinates[0]--;
            }
            break;
        case 2:
            OutNewPlacePosition[0]++;
            if (OutNewPlacePosition[0] >= SnazzCraft::Chunk::Width) { 
                OutNewPlacePosition[0] -= SnazzCraft::Chunk::Width;
                OutChunkCoordinates[0]++;
            }
            break;
        case 3:
            OutNewPlacePosition[2]++;
            if (OutNewPlacePosition[2] >= SnazzCraft::Chunk::Depth) { 
                OutNewPlacePosition[2] -= SnazzCraft::Chunk::Depth;
                OutChunkCoordinates[1]++;
            }
            break;
        case 4:
            OutNewPlacePosition[1]++;
            if (OutNewPlacePosition[1] >= SnazzCraft::Chunk::Height) return false;
            break;
        case 5:
            OutNewPlacePosition[1]--;
            if (OutNewPlacePosition[1] < 0) return false;
            break;
    }
    return true;
}