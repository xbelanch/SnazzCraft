#pragma once

#include "../../glad.h"

#include "../utilities/math.hpp"
#include "../texture/texture.hpp"

#define SNAZZCRAFT_PANEL_LONG_WIDTH   (275)
#define SNAZZCRAFT_PANEL_SHORT_WIDTH  (125)

#define SNAZZCRAFT_PANEL_LONG_HEIGHT  (50)
#define SNAZZCRAFT_PANEL_SHORT_HEIGHT (50)

namespace SnazzCraft
{
    class Panel
    {
    public:
        SnazzCraft::Texture* Texture = nullptr;

        Panel(unsigned int X, unsigned int Y, unsigned int Width, unsigned int Height, SnazzCraft::Texture* Texture); // NOTE: Texture will be deleted when destructor is called

        ~Panel();

        void Draw();

        void UpdateVertices(unsigned int X, unsigned int Y, unsigned int Width, unsigned int Height);

        inline void SwapTextures(SnazzCraft::Texture* NewTexture) // Deletes currently bound texture
        {
            delete this->Texture;
            this->Texture = NewTexture;
        }

    private:
        unsigned int VAO;
        unsigned int VBO;

        void Initiate(unsigned int X, unsigned int Y, unsigned int Width, unsigned int Height);

    };
}

#define SNAZZCRAFT_PANEL_LONG(X, Y, Texture)  new SnazzCraft::Panel(X, Y, SNAZZCRAFT_PANEL_LONG_WIDTH, SNAZZCRAFT_PANEL_LONG_HEIGHT, Texture)
#define SNAZZCRAFT_PANEL_SHORT(X, Y, Texture) new SnazzCraft::Panel(X, Y, SNAZZCRAFT_PANEL_SHORT_WIDTH, SNAZZCRAFT_PANEL_SHORT_HEIGHT, Texture)

