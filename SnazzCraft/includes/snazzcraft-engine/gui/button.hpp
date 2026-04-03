#pragma once

#include <vector>

#include "../../glad.h"
#include "../../glfw3.h"
#include "../../glm/glm.hpp"

#include "../utilities/math.hpp"
#include "panel.hpp"
#include "../input-handler/event.hpp"
#include "../input-handler/data-types.h"
#include "../input-handler/core-callback.hpp"
#include "../texture/texture.hpp"

namespace SnazzCraft
{
    class Button
    {
    public:
        void (*Callback)(SnazzCraft::Event* Event) = nullptr;
        SnazzCraft::Panel* ActivePanel = nullptr;

        uint32_t Position[2];
        uint32_t Dimensions[2];

        Button(uint32_t X, uint32_t Y, uint32_t Width, uint32_t Height);

        ~Button();

        void Draw();

        bool HandleClick(SnazzCraft::Event* Event); // Returns true if callback is called

        inline void SwapPanels(SnazzCraft::Texture* Texture) // Deletes currently bound panel
        {
            delete this->ActivePanel;
            this->ActivePanel = new SnazzCraft::Panel(this->Position[0], this->Position[1], this->Dimensions[0], this->Dimensions[1], Texture);
        }

        inline bool WithinButton(double X, double Y)
        {
            return (X >= static_cast<double>(this->Position[0]) && X <= static_cast<double>(this->Position[0] + this->Dimensions[0]) && Y >= static_cast<double>(this->Position[1]) && Y <= static_cast<double>(this->Position[1] + this->Dimensions[1]));
        }

    private:


    };
} // SnazzCraft

#define SNAZZCRAFT_BUTTON_LONG(X, Y)  new SnazzCraft::Button(X, Y, 275, 50)
#define SNAZZCRAFT_BUTTON_SHORT(X, Y) new SnazzCraft::Button(X, Y, 125, 50)

void SetWorldModeButtonCallback(SnazzCraft::Event* Event);

void SetMainMenuModeButtonCallback(SnazzCraft::Event* Event);
