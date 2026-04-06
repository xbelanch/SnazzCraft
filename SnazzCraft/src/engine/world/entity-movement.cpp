#include "snazzcraft-engine/world/world.hpp"

void SnazzCraft::World::MoveEntity(SnazzCraft::Entity* Entity, const glm::vec3& Rotation, float Distance) const
{
    glm::vec3 OldPosition = Entity->Position;
    Entity->Move(Rotation, Distance);
    glm::vec3 NewPosition = Entity->Position;
    Entity->Position = OldPosition;

    for (uint32_t I = 0; I < 3; I++) {
        float OldCoordinate = Entity->Position[I];

        Entity->Position[I] = NewPosition[I];

        SnazzCraft::Voxel* CollisionVoxel = this->GetCollidingVoxel(Entity->Position, Entity->GetEntityType().EntityHitbox);
        if (CollisionVoxel == nullptr) continue;

        Entity->Position[I] = OldCoordinate;
    }
}

void SnazzCraft::World::MoveEntity(glm::vec3 Translation, SnazzCraft::Entity* Entity) const
{
    for (uint32_t I = 0; I < 3; I++) {
        float OldCoordinate = Entity->Position[I];

        Entity->Position[I] += Translation[I];

        SnazzCraft::Voxel* CollisionVoxel = this->GetCollidingVoxel(Entity->Position, Entity->GetEntityType().EntityHitbox);
        if (CollisionVoxel == nullptr) continue;

        Entity->Position[I] = OldCoordinate;
    }
}