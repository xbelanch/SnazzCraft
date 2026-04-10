#pragma once

#include "snazzcraft-engine/texture/texture.hpp"

namespace SnazzCraft
{
    class TextTexture : public Texture
    {
    public:

    protected:
        std::string Text;

        union 
        {
            struct 
            {
                uint8_t R, G, B;
            };  
            uint8_t Color[3];
        };
        uint8_t Alpha = 0xFF;

        TextTexture(std::string IText, uint8_t IR, uint8_t IG, uint8_t IB);

        virtual void Update();

        virtual ~TextTexture() = default;
        
    private:


    public:
        static constexpr uint8_t PerCharacterWidth = 8u;
        static constexpr uint8_t PerCharacterHeight = 8u;


    };
} // SnazzCraft