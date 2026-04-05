#include "snazzcraft-engine/world/world.hpp"

SnazzCraft::World::LightNode::LightNode(int8_t ILightValue, int32_t IX, int32_t IY, int32_t IZ)
    : X(IX), Y(IY), Z(IZ), LightValue(ILightValue)
{

}

SnazzCraft::World::LightNode::LightNode(int8_t ILightValue, int32_t IPosition[3])
    : X(IPosition[0]), Y(IPosition[1]), Z(IPosition[2]), LightValue(ILightValue)
{
    
}

void SnazzCraft::World::UpdateChunkLighting(SnazzCraft::Chunk* Chunk, bool* UpdatedInputChunk)
{
    Chunk->LightValues.clear();

    std::unordered_set<uint32_t> ChunksToUpdate;
    for (const auto& VoxelPair : Chunk->Voxels) {
        int32_t LightProducingLevel = VoxelPair.second.GetVoxelType().LightProducingLevel;
        if (LightProducingLevel <= 0) continue;

        int32_t Position[3] = {
            static_cast<int32_t>(VoxelPair.second.X) + Chunk->Position[0] * SnazzCraft::Chunk::Width,
            static_cast<int32_t>(VoxelPair.second.Y),
            static_cast<int32_t>(VoxelPair.second.Z) + Chunk->Position[1] * SnazzCraft::Chunk::Depth,
        };
        this->ApplyLightingVoxel(Position, LightProducingLevel, ChunksToUpdate);
    }
    this->ApplySunLightingToChunk(Chunk, ChunksToUpdate);
    
    for (uint32_t I : ChunksToUpdate) {
        auto ChunkIterator = this->Chunks.find(I);
        if (ChunkIterator == this->Chunks.end()) { continue; }

        ChunkIterator->second->UpdateLightingOnVertices();
        ChunkIterator->second->UpdateMesh();
    }

    auto ChunkIterator = ChunksToUpdate.find(INDEX_2D(Chunk->Position[0], Chunk->Position[1], this->Size));
    if (UpdatedInputChunk != nullptr) *UpdatedInputChunk = ChunkIterator != ChunksToUpdate.end();
}

void SnazzCraft::World::ApplySunLightingToChunk(SnazzCraft::Chunk* Chunk, std::unordered_set<uint32_t>& ChunksToUpdate)
{
    for (uint32_t Z = 0; Z < SnazzCraft::Chunk::Depth; Z++) {
    for (uint32_t X = 0; X < SnazzCraft::Chunk::Width; X++) {
        this->ApplySunLightingToColumn(Chunk, X, Z);
    }
    }

    ChunksToUpdate.insert(INDEX_2D(Chunk->Position[0], Chunk->Position[1], this->Size));
}

void SnazzCraft::World::ApplySunLightingToColumn(SnazzCraft::Chunk* Chunk, uint32_t LocalChunkX, uint32_t LocalChunkZ)
{
    for (uint32_t Y = SnazzCraft::Chunk::Height; Y > 0; Y--) {
        uint32_t LightY = Y - 1;
        uint32_t LocalIndex = SnazzCraft::Chunk::LocalVoxelIndex(LocalChunkX, LightY, LocalChunkZ);

        auto LightIterator = Chunk->LightValues.find(LocalIndex);
        if (LightIterator == Chunk->LightValues.end()) {
            Chunk->LightValues.insert_or_assign(LocalIndex, 15); // Sunlight is light level 15    
        } else {
            LightIterator->second = LightIterator->second >= 15 ? LightIterator->second : 15;
        }

        auto VoxelIterator = Chunk->Voxels.find(LocalIndex);
        if (VoxelIterator != Chunk->Voxels.end()) break;
    }
}

void SnazzCraft::World::ApplyLightingVoxel(int32_t LightOrigin[3], int32_t LightProducingLevel, std::unordered_set<uint32_t>& ChunksToUpdate)
{
    auto IsOutsideWorld = [this](int32_t X, int32_t Y, int32_t Z) -> bool
    {
        return X < 0 || Y < 0 || Z < 0 || X >= static_cast<int32_t>(this->Size) * SnazzCraft::Chunk::Width || Y >= SnazzCraft::Chunk::Height || Z >= static_cast<int32_t>(this->Size) * SnazzCraft::Chunk::Depth;
    };

    auto AddLightNodes = [this](std::queue<SnazzCraft::World::LightNode>& Queue, const SnazzCraft::World::LightNode& OriginNode, int LightPropagationDecrease) -> void
    {
        int32_t NewLightValue = OriginNode.LightValue - LightPropagationDecrease;
        if (NewLightValue <= 0) return;

        for (uint8_t I = 0; I < 3; I++) {
            int32_t NewPosition[3] = {
                OriginNode.X,
                OriginNode.Y,
                OriginNode.Z
            };

            // Add in both directons on axis specified by I
            NewPosition[I]--;
            Queue.push(SnazzCraft::World::LightNode(NewLightValue, NewPosition));

            NewPosition[I] += 2;
            Queue.push(SnazzCraft::World::LightNode(NewLightValue, NewPosition));
        }
    };

    std::queue<SnazzCraft::World::LightNode> Queue;
    SnazzCraft::World::LightNode LightOriginNode(LightProducingLevel, LightOrigin);
    Queue.push(LightOriginNode);

    while (!Queue.empty())
    {
        SnazzCraft::World::LightNode CurrentNode = Queue.front();
        Queue.pop();

        if (IsOutsideWorld(CurrentNode.X, CurrentNode.Y, CurrentNode.Z)) continue;

        int32_t ChunkCoordinates[2];
        SnazzCraft::Chunk::GetChunkPosition(CurrentNode.X, CurrentNode.Z, ChunkCoordinates);
        auto ChunkIterator = this->Chunks.find(INDEX_2D(ChunkCoordinates[0], ChunkCoordinates[1], this->Size));
        
        if (ChunkIterator == this->Chunks.end()) {
            this->GenerateChunk(ChunkCoordinates[0], ChunkCoordinates[1], true);
            ChunkIterator = this->Chunks.find(INDEX_2D(ChunkCoordinates[0], ChunkCoordinates[1], this->Size));
        }
        if (ChunkIterator == this->Chunks.end()) continue;

        int32_t Local[3];
        SnazzCraft::Chunk::GetLocalVoxelPosition(CurrentNode.X, CurrentNode.Y, CurrentNode.Z, Local);
        int32_t LightIndex = SnazzCraft::Chunk::LocalVoxelIndex(Local[0], Local[1], Local[2]);
        
        auto ExistingLightIterator = ChunkIterator->second->LightValues.find(LightIndex);
        int32_t CurrentExistingValue = (ExistingLightIterator == ChunkIterator->second->LightValues.end()) ? 0 : ExistingLightIterator->second;

        if (CurrentNode.LightValue > CurrentExistingValue) {
            ChunkIterator->second->LightValues.insert_or_assign(LightIndex, CurrentNode.LightValue);
            ChunksToUpdate.insert(INDEX_2D(ChunkCoordinates[0], ChunkCoordinates[1], this->Size));

            int32_t LocalVoxelPosition[3];
            SnazzCraft::Chunk::GetLocalVoxelPosition(CurrentNode.X, CurrentNode.Y, CurrentNode.Z, LocalVoxelPosition);
            auto VoxelIterator = ChunkIterator->second->Voxels.find(SnazzCraft::Chunk::LocalVoxelIndex(LocalVoxelPosition[0], LocalVoxelPosition[1], LocalVoxelPosition[2]));
            int LightPropogationDecrease = VoxelIterator != ChunkIterator->second->Voxels.end() ? VoxelIterator->second.GetVoxelType().LightPropogationDecrease: 1;

            AddLightNodes(Queue, CurrentNode, LightPropogationDecrease);
        }
    }
}