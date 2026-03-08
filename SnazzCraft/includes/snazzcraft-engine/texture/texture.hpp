#pragma once

#include <fstream>
#include <string>
#include <iostream>

#include "../../glad.h"
#include "../../stb_image.h"
#include "../../glm/glm.hpp"

#include "../utilities/text-file-handling.hpp"

#define SNAZZCRAFT_CHARACTER_BITWISE_WIDTH  (8)
#define SNAZZCRAFT_CHARACTER_BITWISE_HEIGHT (8)

namespace SnazzCraft
{
    class Texture
    {
    public:
        unsigned char* Data = nullptr;
        int Dimensions[2] = { 0, 0 };
        int NRChannels = 4;
        unsigned int* ID = nullptr;

        unsigned int WrapS = GL_REPEAT;
        unsigned int WrapT = GL_REPEAT;

        Texture();

        Texture(const char* FilePath);

        ~Texture();

        void LoadTexture(const char* FilePath);

        void CreateTextureFromString(std::string Text, unsigned char R, unsigned char G, unsigned char B);

        bool OverlayNewTexture(const char* FilePath, int OffsetX, int OffsetY);

        inline bool BindTexture()
        {
            if (this->ID == nullptr) return false;

            glBindTexture(GL_TEXTURE_2D, *this->ID);
            return true;
        }

    private:
        void SetTexture();
        
        inline void DeleteBoundData()
        {
            if (this->Data == nullptr) return;

            delete[] this->Data;
            glDeleteTextures(1, this->ID);
        }
    };

    extern SnazzCraft::Texture* VoxelTextureAtlas;
}



