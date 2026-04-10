#include "snazzcraft-engine/gui/panel/panel.hpp"
#include "snazzcraft-engine/utilities/math.hpp"

#include "glad.h"

constexpr float FullTextureCoordinates[12] = {
    0.0f, 1.0f, // Bottom Left
    0.0f, 0.0f, // Top Left
    1.0f, 0.0f, // Top Right
    1.0f, 0.0f, // Top Right
    0.0f, 1.0f, // Bottom Left
    1.0f, 1.0f  // Bottom Right
};

SnazzCraft::Panel::~Panel()
{
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
}

SnazzCraft::Panel::Panel(uint8_t IX, uint8_t IY, uint32_t IWidth, uint32_t IHeight)
    : X(IX), Y(IY), Width(IWidth), Height(IHeight)
{
    
    
}

void SnazzCraft::Panel::SetVertices()
{
    glBindVertexArray(this->VAO);

    float Vertices[24];
    // Copy position data
    Vertices[SnazzCraft::Index2D(0, 0, 4)] = static_cast<float>(X);          // Bottom Left X
    Vertices[SnazzCraft::Index2D(1, 0, 4)] = static_cast<float>(Y + Height); // Bottom Left Y

    Vertices[SnazzCraft::Index2D(0, 1, 4)] = static_cast<float>(X);          // Top Left X
    Vertices[SnazzCraft::Index2D(1, 1, 4)] = static_cast<float>(Y);          // Top Left Y

    Vertices[SnazzCraft::Index2D(0, 2, 4)] = static_cast<float>(X + Width);  // Top Right X
    Vertices[SnazzCraft::Index2D(1, 2, 4)] = static_cast<float>(Y);          // Top Right Y

    Vertices[SnazzCraft::Index2D(0, 3, 4)] = static_cast<float>(X + Width);  // Top Right X
    Vertices[SnazzCraft::Index2D(1, 3, 4)] = static_cast<float>(Y);          // Top Right Y

    Vertices[SnazzCraft::Index2D(0, 4, 4)] = static_cast<float>(X);          // Bottom Left X
    Vertices[SnazzCraft::Index2D(1, 4, 4)] = static_cast<float>(Y + Height); // Bottom Left Y

    Vertices[SnazzCraft::Index2D(0, 5, 4)] = static_cast<float>(X + Width);  // Bottom Right X
    Vertices[SnazzCraft::Index2D(1, 5, 4)] = static_cast<float>(Y + Height); // Bottom Right Y

    // Copy texture coordinate data
    for (uint8_t Y = 0x00; Y < 0x06; Y++) {
    for (uint8_t X = 0x02; X < 0x04; X++) {
        Vertices[SnazzCraft::Index2D<uint8_t>(X, Y, 4u)] = FullTextureCoordinates[SnazzCraft::Index2D<uint8_t>(X - 2, Y, 2u)];
    }
    }

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    
}

void SnazzCraft::Panel::ProtectedDraw() const
{

}

void SnazzCraft::Panel::Initiate()
{

}
