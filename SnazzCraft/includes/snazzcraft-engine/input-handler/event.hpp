#pragma once

#include "../../glfw3.h"

#include "data.hpp"

namespace SnazzCraft
{
    class Event
    {
    public:
        SnazzCraft::Data* EventData = new SnazzCraft::Data();
        unsigned char Type;

        Event(unsigned char Type);

        virtual ~Event();

    private:


    };
}

