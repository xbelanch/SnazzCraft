#pragma once

#include <vector>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include "../../glm/glm.hpp"
#include "../../glad.h"
#include "../../shader_s.h"

#include "../vertice/voxel-vertice.hpp"

namespace SnazzCraft
{
    class Mesh
    {
    public:
        std::vector<SnazzCraft::VoxelVertice> Vertices;
        std::vector<uint32_t> Indices;
        glm::vec3 ScaleVector = { 1.0f, 1.0f, 1.0f };

        uint32_t VAO;

        Mesh(std::vector<SnazzCraft::VoxelVertice> Vertices, std::vector<uint32_t> Indices); // Use shader before initializing

        virtual ~Mesh();

        virtual void Draw(); 

    private:
        uint32_t VBO;
        uint32_t EBO;

        void Initiate();

    public:
        static SnazzCraft::Mesh* LoadMeshFromObjectFile(const char* FilePath);

    };

    extern SnazzCraft::Mesh* VoxelMesh;
} // SnazzCraft

