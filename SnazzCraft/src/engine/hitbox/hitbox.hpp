#pragma once

#include "../../../includes/glm/glm.hpp"
#include "../../../includes/glm/gtc/matrix_transform.hpp"

#include "../utilities/math.hpp"
#include "../mesh/mesh.hpp"

#define SNAZZCRAFT_HITBOX_TYPE_SIMPLE  (0x00)
#define SNAZZCRAFT_HITBOX_TYPE_COMPLEX (0x01)

const glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

extern std::vector<unsigned int> HitboxIndices;

namespace SnazzCraft
{
    class Hitbox
    {
    public:
        glm::vec3 Position;
        glm::vec3 HalfDimensions;
        glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        unsigned char Type = SNAZZCRAFT_HITBOX_TYPE_SIMPLE;

        Hitbox(glm::vec3 Position, glm::vec3 Dimensions);

        Hitbox(glm::vec3 Position, glm::vec3 Dimensions, glm::vec3 Rotation);

        ~Hitbox();

        bool IsColliding(const Hitbox& CollideHitbox, bool YawOnly = false);

        void SetMesh();

        inline bool IsColliding(const glm::vec3& Point)
        {
            glm::vec3 Delta = Point - this->Position;

            for (unsigned int I = 0; I < 3; I++) {
                float DA = glm::dot(Delta, this->Axies[I]);
                float RA = this->HalfDimensions[I];

                if (glm::abs(DA) > RA) {
                    return false;
                }
            }

            return true;
        }

        inline void UpdateRotation(const glm::vec3& NewRotation)
        {
            this->Rotation = NewRotation;
            
            this->Axies[0] = SnazzCraft::CalculateFrontVector(this->Rotation);
            this->Axies[1] = glm::normalize(glm::cross(this->Axies[0], WorldUp));
            this->Axies[2] = glm::normalize(glm::cross(this->Axies[0], this->Axies[1]));

            this->RotationMatrix = glm::mat4x4(1.0f);
            this->RotationMatrix = glm::rotate(this->RotationMatrix, glm::radians(this->Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            this->RotationMatrix = glm::rotate(this->RotationMatrix, glm::radians(this->Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            this->RotationMatrix = glm::rotate(this->RotationMatrix, glm::radians(this->Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

            for (unsigned int Y = 0; Y < 4; Y++) {
            for (unsigned int X = 0; X < 4; X++) {
                this->AbsoluteRotationMatrix[Y][X] = glm::abs(this->RotationMatrix[Y][X]);
            }
            }

            this->SetMesh();
        }

        inline void UpdatePosition(const glm::vec3& NewPosition)
        {
            if (this->HitboxVertices->empty()) { this->Position = NewPosition; return; }

            delete this->HitboxMesh;

            for (SnazzCraft::Vertice3D& Vertice : *this->HitboxVertices) {
                Vertice.Position += NewPosition - this->Position;
            }

            this->HitboxMesh = new SnazzCraft::Mesh(*this->HitboxVertices, HitboxIndices);
            this->Position = NewPosition;
        }

        inline void Draw()
        {
            if (this->HitboxMesh == nullptr) return;

            this->HitboxMesh->Draw();
        }

    private:
        glm::mat4x4 RotationMatrix;
        glm::mat4x4 AbsoluteRotationMatrix;
        glm::vec3 Axies[3];

        SnazzCraft::Mesh* HitboxMesh = nullptr;
        std::vector<SnazzCraft::Vertice3D>* HitboxVertices = new std::vector<SnazzCraft::Vertice3D>();
    };

    extern SnazzCraft::Hitbox* TestHitbox;
}