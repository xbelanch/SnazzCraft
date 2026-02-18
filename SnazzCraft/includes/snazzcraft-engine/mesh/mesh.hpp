#pragma once

#include <vector>

#include "../../glm/glm.hpp"
#include "../../glad.h"
#include "../../shader_s.h"

#include "../vertice/vertice-3d.hpp"

namespace SnazzCraft
{
    class Mesh
    {
    public:
        std::vector<SnazzCraft::Vertice3D> Vertices;
        std::vector<unsigned int> Indices;
        glm::vec3 ScaleVector = { 1.0f, 1.0f, 1.0f };

        unsigned int VAO;

        Mesh(std::vector<SnazzCraft::Vertice3D> Vertices, std::vector<unsigned int> Indices); // Use shader before initializing

        virtual ~Mesh();

        virtual void Draw(); 

    private:
        unsigned int VBO;
        unsigned int EBO;

        void Initiate();
    };

    extern SnazzCraft::Mesh* VoxelMesh;
}

