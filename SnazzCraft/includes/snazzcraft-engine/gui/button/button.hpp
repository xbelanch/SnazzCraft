#pragma once

#include <stdint.h>

#include "snazzcraft-engine/gui/panel/panel.hpp"

namespace SnazzCraft
{
    class Event;

    class Button
    {
    public:
        virtual ~Button() = default;

        inline void Draw() const
        {
            this->ProtectedDraw();
        }

        inline bool PointWithinButton(double X, double Y) const
        {
            return X >= static_cast<double>(this->X) && X <= static_cast<double>(this->X + this->Width) && Y >= static_cast<double>(this->Y) && Y <= static_cast<double>(this->Y + this->Height);
        }

    protected:
        union 
        {
            struct 
            {
                uint8_t X, Y;
            };
            uint8_t Position[2];
        };

        union 
        {
            struct 
            {   
                uint32_t Width, Height;
            };
            uint32_t Dimensions[2];
        };

        void(*Callback)(SnazzCraft::Event* Event);

        Button(uint8_t IX, uint8_t IY, uint32_t IWidth, uint32_t IHeight);

        virtual void ProtectedDraw() const;

    private:


    };
} // SnazzCraft