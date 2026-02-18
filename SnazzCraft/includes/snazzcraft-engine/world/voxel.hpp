#pragma once

#include <vector>

#include "../../glm/glm.hpp"

#define MAX_BRIGHTNESS (20)

namespace SnazzCraft
{
    class Voxel
    {
    public:
        const static unsigned int Size = 2;

        unsigned int Position[3]; // In local chunk space
        unsigned int ID;
        int LightProducingLevel = 0;
        bool Cullable = true;
        bool Collidable = true;

        /*  
            Front, Left, Right, Back, Top, Bottom
            Represents dimensions
            Value of 0 represents that side is not visible    
        */
        unsigned int Sides[6] = { 1, 1, 1, 1, 1, 1 }; 

        Voxel(unsigned int X, unsigned int Y, unsigned int Z, unsigned int ID);

        Voxel(unsigned int X, unsigned int Y, unsigned int Z, unsigned int ID, bool Cullable);

        Voxel(unsigned int X, unsigned int Y, unsigned int Z, unsigned int ID, bool Cullable, bool Collidable);

        inline unsigned int GetSideCount() const
        {
            unsigned int Count = 0;
            for (unsigned int I = 0; I < 6; I++) {
                if (this->Sides[I] != 0) Count++;
            }

            return Count;
        }

    private:

    };
}

