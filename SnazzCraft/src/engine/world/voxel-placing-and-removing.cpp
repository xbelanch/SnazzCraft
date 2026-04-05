#include"snazzcraft-engine/world/world.hpp"

bool SnazzCraft::World::PlaceVoxel(const glm::vec3& Position, const glm::vec3& Rotation, uint8_t VoxelID)
{
    glm::vec3 EndPosition = Position;
    bool HitVoxel = this->RaycastToVoxel(EndPosition, Rotation, this->PlayerReach);
    if (!HitVoxel || EndPosition == Position) return false;

    int32_t ChunkPosition[2];
    EndPosition /= glm::vec3(static_cast<float>(SnazzCraft::Voxel::Size)); // Now in voxel space 
    SnazzCraft::Chunk::GetChunkPosition(EndPosition, ChunkPosition);

    glm::ivec3 VoxelSpaceInt = EndPosition;
    int32_t LocalVoxelPosition[3];
    SnazzCraft::Chunk::GetLocalVoxelPosition(VoxelSpaceInt.x, VoxelSpaceInt.y, VoxelSpaceInt.z, LocalVoxelPosition);

    uint32_t LocalVoxelIndex = SnazzCraft::Chunk::LocalVoxelIndex(LocalVoxelPosition[0], LocalVoxelPosition[1], LocalVoxelPosition[2]);

    auto ChunkIterator = this->Chunks.find(INDEX_2D(ChunkPosition[0], ChunkPosition[1], this->Size));
    if (ChunkIterator == this->Chunks.end()) return false;

    auto VoxelIterator = ChunkIterator->second->Voxels.find(LocalVoxelIndex);
    if (VoxelIterator != ChunkIterator->second->Voxels.end()) return false;

    ChunkIterator->second->Voxels.insert_or_assign(LocalVoxelIndex, SnazzCraft::Voxel(LocalVoxelPosition[0], LocalVoxelPosition[1], LocalVoxelPosition[2], VoxelID));

    SnazzCraft::Voxel* CollidingVoxel = this->GetCollidingVoxel(SnazzCraft::Player->EntityHitbox);
    if (CollidingVoxel != nullptr && SnazzCraft::Chunk::LocalVoxelIndex(*CollidingVoxel) == LocalVoxelIndex) {
        ChunkIterator->second->Voxels.erase(LocalVoxelIndex);
        return false;
    }

    ChunkIterator->second->CullVoxelFaces();
    ChunkIterator->second->UpdateVerticesAndIndices();
    this->UpdateChunkLighting(ChunkIterator->second, nullptr);
    
    return true;
}

bool SnazzCraft::World::RaycastToVoxel(glm::vec3& Position, const glm::vec3& Rotation, float MaxDistance)
{
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

    float DistanceTraveled = 0.0f;
    while (DistanceTraveled < MaxDistance)
    {
        if (this->GetCollidingVoxel(glm::vec3(MapPosition) * VoxelSize) != nullptr) {
            Position = RayOrigin + (RayDirection * DistanceTraveled);
            return true;
        }

        if (SideDistance.x < SideDistance.y && SideDistance.x < SideDistance.z) {
            DistanceTraveled = SideDistance.x;
            SideDistance.x += DeltaDistance.x;
            MapPosition.x += Step.x;
        } 
        else if (SideDistance.y < SideDistance.z) {
            DistanceTraveled = SideDistance.y;
            SideDistance.y += DeltaDistance.y;
            MapPosition.y += Step.y;
        } 
        else {
            DistanceTraveled = SideDistance.z;
            SideDistance.z += DeltaDistance.z;
            MapPosition.z += Step.z;
        }
    }

    Position = RayOrigin + (RayDirection * MaxDistance);
    return false;
}