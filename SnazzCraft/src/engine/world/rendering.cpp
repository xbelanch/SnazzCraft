#include "snazzcraft-engine/world/world.hpp"
#include "snazzcraft-engine/core/core.hpp"
#include "snazzcraft-engine/utilities/math.hpp"
#include "snazzcraft-engine/texture/texture.hpp"
#include "snazzcraft-engine/mesh/mesh.hpp"
#include "snazzcraft-engine/world/chunk.hpp"

void SnazzCraft::World::Render() const
{
    // Render voxels in chunks & voxel placement display
    SnazzCraft::VoxelTextureAtlas->BindTexture();

    SnazzCraft::ModelMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(SnazzCraft::ModelLock, 1, GL_FALSE, glm::value_ptr(SnazzCraft::ModelMatrix));
    this->RenderChunks();

    this->RenderVoxelPlacementDisplay();
    
    // Render entities
    this->RenderAllEntities();
}

void SnazzCraft::World::RenderAllEntities() const
{
    auto RenderEntity = [](SnazzCraft::Entity* Entity) -> void
    {
        const SnazzCraft::EntityType& EntityType = Entity->GetEntityType();

        SnazzCraft::ModelMatrix = glm::translate(glm::mat4(1.0f), Entity->Position);
        
        SnazzCraft::ModelMatrix = glm::rotate(SnazzCraft::ModelMatrix, glm::radians(Entity->Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        SnazzCraft::ModelMatrix = glm::rotate(SnazzCraft::ModelMatrix, glm::radians(-Entity->Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        SnazzCraft::ModelMatrix = glm::rotate(SnazzCraft::ModelMatrix, glm::radians(Entity->Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        SnazzCraft::ModelMatrix = glm::scale(SnazzCraft::ModelMatrix, EntityType.EntityMesh->ScaleVector);
        
        glUniformMatrix4fv(SnazzCraft::ModelLock, 1, GL_FALSE, glm::value_ptr(SnazzCraft::ModelMatrix));
        EntityType.EntityTexture->BindTexture();
        EntityType.EntityMesh->Draw();
    };

    RenderEntity(SnazzCraft::Player);
    for (SnazzCraft::Entity* Entity : this->Entities) {
        RenderEntity(Entity);
    }
}

void SnazzCraft::World::RenderChunks() const
{ 
    int32_t PlayerChunkPosition[2];
    SnazzCraft::Chunk::GetChunkPosition(Player->Position, PlayerChunkPosition);

    for (int32_t X = PlayerChunkPosition[0] - static_cast<int32_t>(this->RenderDistance); X <= PlayerChunkPosition[0] + static_cast<int32_t>(this->RenderDistance); X++) {
    for (int32_t Z = PlayerChunkPosition[1] - static_cast<int32_t>(this->RenderDistance); Z <= PlayerChunkPosition[1] + static_cast<int32_t>(this->RenderDistance); Z++) {
        if (X < 0 || X >= static_cast<int32_t>(this->Size) || Z < 0 || Z >= static_cast<int32_t>(this->Size)) continue;

        auto ChunkIterator = this->Chunks.find(SnazzCraft::Index2D<int32_t>(X, Z, static_cast<int32_t>(this->Size)));
        if (ChunkIterator == this->Chunks.end()) continue;

        if (ChunkIterator->second->ChunkMesh == nullptr) continue; 
        ChunkIterator->second->ChunkMesh->Draw();
    }
    }
}

void SnazzCraft::World::RenderVoxelPlacementDisplay() const
{
    if (!this->ShouldRenderVoxelPlacementDisplay) return;

    SnazzCraft::ModelMatrix = glm::translate(glm::mat4(1.0f), this->VoxelPlacementDisplayPosition);
    SnazzCraft::ModelMatrix = glm::scale(SnazzCraft::ModelMatrix, this->VoxelPlacementDisplayMesh->ScaleVector);
    glUniformMatrix4fv(SnazzCraft::ModelLock, 1, GL_FALSE, glm::value_ptr(SnazzCraft::ModelMatrix));
    this->VoxelPlacementDisplayMesh->Draw();
}

