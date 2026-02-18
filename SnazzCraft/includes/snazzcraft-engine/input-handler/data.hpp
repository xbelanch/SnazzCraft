#pragma once

#include <vector>
#include <string>

#include "../../glm/glm.hpp"

#include "data-types.h"

namespace SnazzCraft
{
    struct Data
    {
    public:
        std::vector<void*> Items;
        std::vector<unsigned char> Types;

        ~Data();

        void* AccessDataType(unsigned char Type); // Returns first occurrence of data with specified type

    private:


    };
}
