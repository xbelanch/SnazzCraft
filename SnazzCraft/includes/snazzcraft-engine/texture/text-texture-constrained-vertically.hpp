#pragma once

#include <string>
#include <stdint.h>

#include "snazzcraft-engine/texture/text-texture.hpp"

namespace SnazzCraft
{
    class TextTextureConstrainedVertically : public TextTexture
    {
    public:
        TextTextureConstrainedVertically(std::string IText, uint8_t IR, uint8_t IG, uint8_t IB);

        virtual void Update() override;

        virtual inline bool BindTexture() const override
        {
            if (this->ID == nullptr) return false;

            glBindTexture(GL_TEXTURE_2D, *this->ID);
            return true;
        }
    };


} // SnazzCraft