#pragma once

#include <stdint.h>

namespace SnazzCraft
{
    class Panel
    {
    public:  
        inline void Draw() const
        {
            this->ProtectedDraw();
        }

        virtual ~Panel();

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

        /*
        Vertice:
            X, Y, U, V
        */
        uint32_t VAO, VBO;
    

        Panel(uint8_t IX, uint8_t IY, uint32_t IWidth, uint32_t IHeight);

        virtual void SetVertices(); 

        virtual void ProtectedDraw() const;

        virtual void Initiate();

    private:


    };
};