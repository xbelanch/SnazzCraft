#include "snazzcraft-engine/gui/panel.hpp"

const float FullTextureCoordinates[12] = {
    0.0f, 1.0f, // Bottom Left
    0.0f, 0.0f, // Top Left
    1.0f, 0.0f, // Top Right
    1.0f, 0.0f, // Top Right
    0.0f, 1.0f, // Bottom Left
    1.0f, 1.0f  // Bottom Right
};

const unsigned char FullScreenIndexesX[3] = {
    4, 6, 10
};

const unsigned char FullScreenIndexesY[3] = {
    1, 9, 11
};

SnazzCraft::Panel::Panel(unsigned int X, unsigned int Y, unsigned int Width, unsigned int Height, SnazzCraft::Texture* Texture)
{
    this->Texture = Texture;

    this->Initiate(X, Y, Width, Height);
}

SnazzCraft::Panel::~Panel()
{
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
   
    delete this->Texture;
}

void SnazzCraft::Panel::Draw()
{
    if (this->Texture == nullptr) return;

    if (!this->Texture->BindTexture()) return;

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SnazzCraft::Panel::UpdateVertices(unsigned int X, unsigned int Y, unsigned int Width, unsigned int Height) 
{
    glBindVertexArray(this->VAO);

    float Vertices[24];
    // Copy position data
    Vertices[INDEX_2D(0, 0, 4)] = static_cast<float>(X);          // Bottom Left X
    Vertices[INDEX_2D(1, 0, 4)] = static_cast<float>(Y + Height); // Bottom Left Y
    Vertices[INDEX_2D(0, 1, 4)] = static_cast<float>(X);          // Top Left X
    Vertices[INDEX_2D(1, 1, 4)] = static_cast<float>(Y);          // Top Left Y
    Vertices[INDEX_2D(0, 2, 4)] = static_cast<float>(X + Width);  // Top Right X
    Vertices[INDEX_2D(1, 2, 4)] = static_cast<float>(Y);          // Top Right Y
    Vertices[INDEX_2D(0, 3, 4)] = static_cast<float>(X + Width);  // Top Right X
    Vertices[INDEX_2D(1, 3, 4)] = static_cast<float>(Y);          // Top Right Y
    Vertices[INDEX_2D(0, 4, 4)] = static_cast<float>(X);          // Bottom Left X
    Vertices[INDEX_2D(1, 4, 4)] = static_cast<float>(Y + Height); // Bottom Left Y
    Vertices[INDEX_2D(0, 5, 4)] = static_cast<float>(X + Width);  // Bottom Right X
    Vertices[INDEX_2D(1, 5, 4)] = static_cast<float>(Y + Height); // Bottom Right Y

    // Copy texture coordinate data
    for (unsigned int Y = 0; Y < 6; Y++) {
    for (unsigned int X = 2; X < 4; X++) {
        Vertices[INDEX_2D(X, Y, 4)] = FullTextureCoordinates[INDEX_2D(X - 2, Y, 2)];
    }
    }

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void SnazzCraft::Panel::Initiate(unsigned int X, unsigned int Y, unsigned int Width, unsigned int Height)
{
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    this->UpdateVertices(X, Y, Width, Height);

    glBindVertexArray(this->VAO);

    // Position attribute (location = 0)
    glVertexAttribPointer(
        0,                          // location
        2,                          // vec2
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),          // STRIDE 
        (void*)0                    // offset
    );
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute (location = 1)
    glVertexAttribPointer(
        1,                          // location
        2,                          // vec2
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),          // 
        (void*)(2 * sizeof(float))  // offset after x,y
    );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
