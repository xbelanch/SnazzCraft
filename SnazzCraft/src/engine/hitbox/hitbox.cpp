#include "snazzcraft-engine/hitbox/hitbox.hpp"

SnazzCraft::Hitbox::Hitbox(glm::vec3 Position, glm::vec3 Dimensions)
{
    this->Position = Position;
    this->UpdateDimensions(Dimensions);
}

