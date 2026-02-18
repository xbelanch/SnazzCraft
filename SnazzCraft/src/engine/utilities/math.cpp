#include "snazzcraft-engine/utilities/math.hpp"

glm::vec3 SnazzCraft::CalculateFrontVector(const glm::vec3& Rotation, bool Normalize)
{
    float RadYaw   = glm::radians(Rotation.y);
    float RadPitch = glm::radians(Rotation.z);

    float CosYaw   = cos(RadYaw);
    float SinYaw   = sin(RadYaw);
    float CosPitch = cos(RadPitch);
    float SinPitch = sin(RadPitch);

    glm::vec3 Front;
    Front.x = CosYaw * CosPitch;
    Front.y = SinPitch;
    Front.z = SinYaw * CosPitch;

    if (Normalize) Front   = glm::normalize(Front);
    
    return Front;
}

void SnazzCraft::MoveVector3D(glm::vec3& Vector, const glm::vec3& Rotation, float Distance)
{
    glm::vec3 Front = SnazzCraft::CalculateFrontVector(Rotation, true);
    Vector += Front * Distance;
}

void SnazzCraft::MoveVector3DWithFront(glm::vec3& Vector, const glm::vec3& Front, float Distance)
{
    Vector += Front * Distance;
}
