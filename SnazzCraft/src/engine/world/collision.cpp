#include "snazzcraft-engine/world/world.hpp"

SnazzCraft::Voxel* SnazzCraft::World::GetCollidingVoxel(const glm::vec3& Position, const SnazzCraft::Hitbox* Hitbox) const
{
    const int32_t ChunkX = static_cast<int32_t>(Position.x / (SnazzCraft::Chunk::Width * SnazzCraft::Voxel::Size));
    const int32_t ChunkZ = static_cast<int32_t>(Position.z / (SnazzCraft::Chunk::Depth * SnazzCraft::Voxel::Size));

    for (int32_t X = ChunkX - 1; X <= ChunkX + 1; X++) {
    for (int32_t Z = ChunkZ - 1; Z <= ChunkZ + 1; Z++) {
        if (X < 0 || Z < 0 || X >= static_cast<int32_t>(this->Size) || Z >= static_cast<int32_t>(this->Size)) continue;
        
        auto ChunkIterator = this->Chunks.find(INDEX_2D(X, Z, this->Size));
        if (ChunkIterator == this->Chunks.end()) continue;

        SnazzCraft::Voxel* CollisionVoxel = ChunkIterator->second->GetCollidingVoxel(Position, Hitbox);
        if (CollisionVoxel != nullptr) return CollisionVoxel;
    }
    }

    return nullptr;
}

SnazzCraft::Voxel* SnazzCraft::World::GetCollidingVoxel(const glm::vec3& Position) const
{
    const int32_t ChunkX = static_cast<int32_t>(Position.x / (SnazzCraft::Chunk::Width * SnazzCraft::Voxel::Size));
    const int32_t ChunkZ = static_cast<int32_t>(Position.z / (SnazzCraft::Chunk::Depth * SnazzCraft::Voxel::Size));

    if (ChunkX < 0 || ChunkZ < 0 || ChunkX >= static_cast<int32_t>(this->Size) || ChunkZ >= static_cast<int32_t>(this->Size)) return nullptr;

    auto ChunkIterator = this->Chunks.find(INDEX_2D(ChunkX, ChunkZ, this->Size));
    if (ChunkIterator == this->Chunks.end()) return nullptr;

    SnazzCraft::Voxel* CollisionVoxel = ChunkIterator->second->GetCollidingVoxel(Position);
    if (CollisionVoxel != nullptr) return CollisionVoxel;

    return nullptr;
}