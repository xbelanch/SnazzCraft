#pragma once

#include <stdint.h>
#include <string>

#include "snazzcraft-engine/gui/panel/panel.hpp"

namespace SnazzCraft
{
    class Texture;

    class PanelWithTexture : public Panel
    {
        public:
            PanelWithTexture(uint8_t IX, uint8_t IY, uint32_t IWidth, uint32_t IHeight, std::string FilePathToTexture);

            virtual ~PanelWithTexture();

        protected:
            SnazzCraft::Texture* Texture;

            virtual void ProtectedDraw() const override;

            virtual void Initiate() override;

        private:


    };
} // SnazzCraft