#include "snazzcraft-engine/mesh/mesh.hpp"

SnazzCraft::Mesh* SnazzCraft::VoxelMesh = nullptr;

SnazzCraft::Mesh::Mesh(std::vector<SnazzCraft::Vertice3D> Vertices, std::vector<unsigned int> Indices)
{
	this->Vertices = Vertices;
	this->Indices = Indices;

    this->Initiate();
}

SnazzCraft::Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
}

void SnazzCraft::Mesh::Draw()
{
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SnazzCraft::Mesh::Initiate()
{
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    // Bind VAO first
    glBindVertexArray(this->VAO);

    // VBO: upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->Vertices.size() * sizeof(SnazzCraft::Vertice3D), this->Vertices.data(), GL_STATIC_DRAW);

    // EBO: upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->Indices.size() * sizeof(unsigned int), this->Indices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers

    // layout (location = 0) ? vec3 position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SnazzCraft::Vertice3D), (void*)0);
    glEnableVertexAttribArray(0);

    // layout (location = 1) ? vec2 texCoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SnazzCraft::Vertice3D), (void*)offsetof(SnazzCraft::Vertice3D, SnazzCraft::Vertice3D::TextureCoordinate));
    glEnableVertexAttribArray(1);

    // layout (location = 2) ? float brightness
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(SnazzCraft::Vertice3D), (void*)offsetof(SnazzCraft::Vertice3D, SnazzCraft::Vertice3D::Brightness));
    glEnableVertexAttribArray(2);

    // Unbind (optional safety)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}




