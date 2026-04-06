#include "snazzcraft-engine/world/world.hpp"

SnazzCraft::World* SnazzCraft::CurrentWorld = nullptr;

SnazzCraft::World::World(std::string IName, uint32_t ISize, int32_t ISeed)
    : Name(IName), Size(ISize), Seed(ISeed), Chunks(std::unordered_map<uint32_t, SnazzCraft::Chunk*>()), VoxelPlacementDisplayPosition(glm::vec3(0.0f))
{
    this->WorldHeightMap = new SnazzCraft::HeightMap(this->Size * SnazzCraft::Chunk::Width, -SnazzCraft::Chunk::MaxOceanDepth, SnazzCraft::Chunk::Height - SnazzCraft::Chunk::OceanLevel, this->Seed, 1.0, 0.5, 2.0, 6);

    this->VoxelPlacementDisplayMesh = new SnazzCraft::Mesh(SnazzCraft::EngineVoxelTextureApplier->GetTexturedVertices(SnazzCraft::Voxel(0, 0, 0, ID_VOXEL_BARRIER)), SnazzCraft::VoxelMesh->Indices);
}

SnazzCraft::World::~World()
{
    for (auto& ChunkPair : this->Chunks) {
        delete ChunkPair.second;
    }

    for (SnazzCraft::Entity* Entity : this->Entities) {
        delete Entity;
    }

    delete this->WorldHeightMap;
    delete this->VoxelPlacementDisplayMesh;
}











