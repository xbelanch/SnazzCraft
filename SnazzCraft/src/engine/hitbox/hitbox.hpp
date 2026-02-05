#pragma once

#include "../../../includes/glad.h"
#include "../../../includes/glm/glm.hpp"
#include "../../../includes/glm/gtc/matrix_transform.hpp"

#include "../utilities/math/math.hpp"
#include "../mesh/mesh.hpp"
#include "../texture/solid-colors/solid-colors.hpp"
#include "../world/voxel/voxel.hpp"

const std::vector<unsigned int> HitboxIndices = {
    0, 1, 2, 2, 1, 3,
    4, 6, 5, 5, 6, 7,
    0, 2, 4, 4, 2, 6,
    1, 5, 3, 3, 5, 7,
    0, 4, 1, 1, 4, 5,
    2, 3, 6, 6, 3, 7
};

const glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

namespace SnazzCraft
{
    class Hitbox
    {
    public:
        glm::vec3 Position;
        glm::vec3 HalfDimensions;
        glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        std::string HitboxColor = SNAZZCRAFT_COLOR_WHITE;

        Hitbox(glm::vec3 Position, glm::vec3 Dimensions);

        Hitbox(glm::vec3 Position, glm::vec3 Dimensions, glm::vec3 Rotation);

        ~Hitbox();

        void SetMesh();

        bool IsColliding(const Hitbox& CollideHitbox, bool YawOnly); const

        inline bool IsCollidingVoxel(const glm::vec3& VoxelWorldSpacePosition) const
        {
            glm::vec3 Delta = VoxelWorldSpacePosition - this->Position;

            for (unsigned int I = 0; I < 3; I++) {
                float DA = glm::dot(Delta, this->Axies[I]);
                float RA = this->HalfDimensions[I];

                if (glm::abs(DA) > (RA + 1.0f)) return false; // Voxel half dimension is always 1
            }

            return true;
        }

        inline bool IsColliding(const glm::vec3& Point) const
        {
            glm::vec3 Delta = Point - this->Position;

            for (unsigned int I = 0; I < 3; I++) {
                float DA = glm::dot(Delta, this->Axies[I]);
                float RA = this->HalfDimensions[I];

                if (glm::abs(DA) > RA) return false;
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
            if (this->HitboxVertices->empty() || this->HitboxMesh == nullptr) this->SetMesh(); 

            delete this->HitboxMesh;
            this->HitboxMesh = nullptr;

            for (SnazzCraft::Vertice3D& Vertice : *this->HitboxVertices) {
                Vertice.Position += NewPosition - this->Position;
            }

            this->HitboxMesh = new SnazzCraft::Mesh(*this->HitboxVertices, HitboxIndices);
            this->Position = NewPosition;
        }

        inline void Draw(bool OverrideCullFace)
        {
            if (this->HitboxMesh == nullptr || this->HitboxTexture == nullptr) return;

            glEnable(GL_DEPTH_TEST);

            if (!OverrideCullFace) {
                glCullFace(GL_FRONT); 
                glFrontFace(GL_CW);  
                glEnable(GL_CULL_FACE);
            } else {
                glDisable(GL_CULL_FACE);
            }

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            this->HitboxTexture->BindTexture();
            this->HitboxMesh->Draw();
        }

        inline void UpdateColor(const std::string& NewColor)
        {
            this->HitboxColor = NewColor;

            delete this->HitboxTexture;
            this->HitboxTexture = SnazzCraft::GenerateSolidColorTexture(this->HitboxColor);
        }

    private:
        glm::mat4x4 RotationMatrix = glm::mat4x4(1.0f);
        glm::mat4x4 AbsoluteRotationMatrix = glm::mat4x4(1.0f);
        glm::vec3 Axies[3];

        SnazzCraft::Mesh* HitboxMesh = nullptr;
        std::vector<SnazzCraft::Vertice3D>* HitboxVertices = new std::vector<SnazzCraft::Vertice3D>();
        SnazzCraft::Texture* HitboxTexture = nullptr;
    };

    extern SnazzCraft::Hitbox* TestHitbox;
}