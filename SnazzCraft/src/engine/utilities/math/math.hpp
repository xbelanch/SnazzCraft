#pragma once

#include "../../../../includes/glm/glm.hpp"

#define INDEX_2D(X, Y, Width) ((Y) * (Width) + (X))
#define INDEX_3D(X, Y, Z, Width, Height) (X + Y * Width + Z * Width * Height)

namespace SnazzCraft
{
    glm::vec3 CalculateFrontVector(const glm::vec3& Rotation, bool Normalize);
}   




