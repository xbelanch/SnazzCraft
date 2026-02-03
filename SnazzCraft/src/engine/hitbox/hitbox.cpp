#include "hitbox.hpp"

SnazzCraft::Hitbox* SnazzCraft::TestHitbox = nullptr;

std::vector<unsigned int> HitboxIndices = {
    0, 1, 2, 2, 1, 3,
    4, 6, 5, 5, 6, 7,
    0, 2, 4, 4, 2, 6,
    1, 5, 3, 3, 5, 7,
    0, 4, 1, 1, 4, 5,
    2, 3, 6, 6, 3, 7
};

SnazzCraft::Hitbox::Hitbox(glm::vec3 Position, glm::vec3 Dimensions)
{
    this->Position = Position;
    this->HalfDimensions = Dimensions / 2.0f;
    this->UpdateRotation(this->Rotation);
}

SnazzCraft::Hitbox::Hitbox(glm::vec3 Position, glm::vec3 Dimensions, glm::vec3 Rotation)
{
    this->Position = Position;
    this->HalfDimensions = Dimensions / 2.0f;
    this->UpdateRotation(Rotation);
}

SnazzCraft::Hitbox::~Hitbox()
{
    delete this->HitboxMesh;
    delete this->HitboxVertices;
}

bool SnazzCraft::Hitbox::IsColliding(const SnazzCraft::Hitbox& CollideHitbox, bool YawOnly)
{
    glm::vec3 Delta = CollideHitbox.Position - this->Position;

    float DA[3] = {
        glm::dot(Delta, this->Axies[0]),
        glm::dot(Delta, this->Axies[1]),
        glm::dot(Delta, this->Axies[2])
    };

    float RA, RB;
    for (unsigned int I = 0; I < 3; I++) {
        RA = this->HalfDimensions[I];
        RB = CollideHitbox.HalfDimensions.x * this->AbsoluteRotationMatrix[I][0] +
             CollideHitbox.HalfDimensions.y * this->AbsoluteRotationMatrix[I][1] +
             CollideHitbox.HalfDimensions.z * this->AbsoluteRotationMatrix[I][2];

        if (glm::abs(DA[I]) > (RA + RB)) {
            return false;
        }
    }

    for (unsigned int I = 0; I < 3; I++) {
        RA = this->HalfDimensions.x * CollideHitbox.AbsoluteRotationMatrix[0][I] +
             this->HalfDimensions.y * CollideHitbox.AbsoluteRotationMatrix[1][I] +
             this->HalfDimensions.z * CollideHitbox.AbsoluteRotationMatrix[2][I];
        RB = CollideHitbox.HalfDimensions[I];

        float DAB = glm::dot(Delta, CollideHitbox.Axies[I]);

        if (glm::abs(DAB) > (RA + RB)) {
            return false;
        }
    }

    if (YawOnly) return true;

    // Test cross products of axies

    // A0 x B0
    RA = this->HalfDimensions.y * this->AbsoluteRotationMatrix[2][0] + this->HalfDimensions.z * this->AbsoluteRotationMatrix[1][0];
    RB = CollideHitbox.HalfDimensions.y * CollideHitbox.AbsoluteRotationMatrix[0][2] + CollideHitbox.HalfDimensions.z * CollideHitbox.AbsoluteRotationMatrix[0][1];
    float D = glm::abs(DA[2] * this->RotationMatrix[1][0] - DA[1] * this->RotationMatrix[2][0]);
    if (D > (RA + RB)) return false;

    // A0 x B1
    RA = this->HalfDimensions.y * this->AbsoluteRotationMatrix[2][1] + this->HalfDimensions.z * this->AbsoluteRotationMatrix[1][1];
    RB = CollideHitbox.HalfDimensions.x * CollideHitbox.AbsoluteRotationMatrix[0][2] + CollideHitbox.HalfDimensions.z * CollideHitbox.AbsoluteRotationMatrix[0][0];
    D = glm::abs(DA[2] * this->RotationMatrix[1][1] - DA[1] * this->RotationMatrix[2][1]);
    if (D > (RA + RB)) return false;

    // A0 x B2
    RA = this->HalfDimensions.y * this->AbsoluteRotationMatrix[2][2] + this->HalfDimensions.z * this->AbsoluteRotationMatrix[1][2];
    RB = CollideHitbox.HalfDimensions.x * CollideHitbox.AbsoluteRotationMatrix[0][1] + CollideHitbox.HalfDimensions.y * CollideHitbox.AbsoluteRotationMatrix[0][0];
    D = glm::abs(DA[2] * this->RotationMatrix[1][2] - DA[1] * this->RotationMatrix[2][2]);
    if (D > (RA + RB)) return false;

    // A1 x B0
    RA = this->HalfDimensions.x * this->AbsoluteRotationMatrix[2][0] + this->HalfDimensions.z * this->AbsoluteRotationMatrix[0][0];
    RB = CollideHitbox.HalfDimensions.y * CollideHitbox.AbsoluteRotationMatrix[1][2] + CollideHitbox.HalfDimensions.z * CollideHitbox.AbsoluteRotationMatrix[1][1];
    D = glm::abs(DA[0] * this->RotationMatrix[2][0] - DA[2] * this->RotationMatrix[0][0]);
    if (D > (RA + RB)) return false;

    // A1 x B1
    RA = this->HalfDimensions.x * this->AbsoluteRotationMatrix[2][1] + this->HalfDimensions.z * this->AbsoluteRotationMatrix[0][1];
    RB = CollideHitbox.HalfDimensions.x * CollideHitbox.AbsoluteRotationMatrix[1][2] + CollideHitbox.HalfDimensions.z * CollideHitbox.AbsoluteRotationMatrix[1][0];
    D = glm::abs(DA[0] * this->RotationMatrix[2][1] - DA[2] * this->RotationMatrix[0][1]);
    if (D > (RA + RB)) return false;

    // A1 x B2
    RA = this->HalfDimensions.x * this->AbsoluteRotationMatrix[2][2] + this->HalfDimensions.z * this->AbsoluteRotationMatrix[0][2];
    RB = CollideHitbox.HalfDimensions.x * CollideHitbox.AbsoluteRotationMatrix[1][1] + CollideHitbox.HalfDimensions.y * CollideHitbox.AbsoluteRotationMatrix[1][0];
    D = glm::abs(DA[0] * this->RotationMatrix[2][2] - DA[2] * this->RotationMatrix[0][2]);
    if (D > (RA + RB)) return false;

    // A2 x B0
    RA = this->HalfDimensions.x * this->AbsoluteRotationMatrix[1][0] + this->HalfDimensions.y * this->AbsoluteRotationMatrix[0][0];
    RB = CollideHitbox.HalfDimensions.y * CollideHitbox.AbsoluteRotationMatrix[2][2] + CollideHitbox.HalfDimensions.z * CollideHitbox.AbsoluteRotationMatrix[2][1];
    D = glm::abs(DA[1] * this->RotationMatrix[0][0] - DA[0] * this->RotationMatrix[1][0]);
    if (D > (RA + RB)) return false;

    // A2 x B1
    RA = this->HalfDimensions.x * this->AbsoluteRotationMatrix[1][1] + this->HalfDimensions.y * this->AbsoluteRotationMatrix[0][1];
    RB = CollideHitbox.HalfDimensions.x * CollideHitbox.AbsoluteRotationMatrix[2][2] + CollideHitbox.HalfDimensions.z * CollideHitbox.AbsoluteRotationMatrix[2][0];
    D = glm::abs(DA[1] * this->RotationMatrix[0][1] - DA[0] * this->RotationMatrix[1][1]);
    if (D > (RA + RB)) return false;

    // A2 x B2
    RA = this->HalfDimensions.x * this->AbsoluteRotationMatrix[1][2] + this->HalfDimensions.y * this->AbsoluteRotationMatrix[0][2];
    RB = CollideHitbox.HalfDimensions.x * CollideHitbox.AbsoluteRotationMatrix[2][1] + CollideHitbox.HalfDimensions.y * CollideHitbox.AbsoluteRotationMatrix[2][0];
    D = glm::abs(DA[1] * this->RotationMatrix[0][2] - DA[0] * this->RotationMatrix[1][2]);
    if (D > (RA + RB)) return false;

    return true;
}

void SnazzCraft::Hitbox::SetMesh()
{
    delete this->HitboxMesh;

    this->HitboxVertices->clear();

    for (unsigned int X = 0; X < 2; X++) {
    for (unsigned int Y = 0; Y < 2; Y++) {
    for (unsigned int Z = 0; Z < 2; Z++) {
        glm::vec4 Position = glm::vec4(
            (X == 0 ? -this->HalfDimensions.x : this->HalfDimensions.x),
            (Y == 0 ? -this->HalfDimensions.y : this->HalfDimensions.y),
            (Z == 0 ? -this->HalfDimensions.z : this->HalfDimensions.z),
            1.0f
        );

        Position = this->RotationMatrix * Position;
        Position += glm::vec4(this->Position, 0.0f);

        this->HitboxVertices->push_back(SnazzCraft::Vertice3D(glm::vec3(Position), glm::vec2(0.1f, 0.1f)));
    }
    }   
    }

    this->HitboxMesh = new SnazzCraft::Mesh(*this->HitboxVertices, HitboxIndices);
}
