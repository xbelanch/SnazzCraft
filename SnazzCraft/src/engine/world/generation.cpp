#include "snazzcraft-engine/world/world.hpp"
#include "snazzcraft-engine/world/chunk.hpp"
#include "snazzcraft-engine/utilities/math.hpp"

void SnazzCraft::World::GenerateChunk(uint32_t X, uint32_t Z, bool ApplyLighting)
{
    if (X >= SnazzCraft::World::Size || Z >= SnazzCraft::World::Size) return;

    uint32_t ChunkIndex = SnazzCraft::Index2D(X, Z, this->Size);
    auto Iterator = this->Chunks.find(ChunkIndex);
    if (Iterator != this->Chunks.end()) return;
    
    SnazzCraft::Chunk* NewChunk = new SnazzCraft::Chunk(X, Z);

    NewChunk->Generate(this->WorldHeightMap, this->Size * SnazzCraft::Chunk::Width);
    NewChunk->CullVoxelFaces();
    NewChunk->UpdateVerticesAndIndices();   

    this->Chunks[ChunkIndex] = NewChunk;
    bool UpdatedChunk;
    if (ApplyLighting) { 
        this->UpdateChunkLighting(NewChunk, &UpdatedChunk); 
    }

    if (!UpdatedChunk) {
        NewChunk->UpdateLightingOnVertices(this);
        NewChunk->UpdateMesh();
    }
}

SnazzCraft::World* SnazzCraft::World::CreateWorld(std::string Name, uint32_t Size, int32_t Seed)
{
    uint32_t GenerateSize = Size == 0 || Size > SnazzCraft::World::MaxSize ? 
        SnazzCraft::World::MaxSize : 
        Size;

    SnazzCraft::World* NewWorld = new SnazzCraft::World(Name, GenerateSize, Seed);

    for (uint32_t X = 0; X < NewWorld->Size; X++) {
    for (uint32_t Z = 0; Z < NewWorld->Size; Z++) {
        NewWorld->GenerateChunk(X, Z, true);
    } 
    }
    
    return NewWorld;
}

void SnazzCraft::World::UpdateChunkVerticeLightingAndMesh(uint32_t Index)
{
    auto ChunkIterator = this->Chunks.find(Index);
    if (ChunkIterator == this->Chunks.end()) return;

    this->UpdateChunkVerticeLightingAndMesh(ChunkIterator->second);
}

void SnazzCraft::World::UpdateChunkVerticeLightingAndMesh(SnazzCraft::Chunk* Chunk)
{
    Chunk->UpdateLightingOnVertices(this);
    Chunk->UpdateMesh();
}
