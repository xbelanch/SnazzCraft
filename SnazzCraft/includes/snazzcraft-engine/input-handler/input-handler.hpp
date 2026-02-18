#pragma once

#include <vector>

#include "../../glfw3.h"

#include "event.hpp"
#include "keys.h"
#include "events.h"
#include "data-types.h"

namespace SnazzCraft
{
    class InputHandler
    {
    public:
        std::vector<SnazzCraft::Event*> EventQueue;
        void (*Callback)(SnazzCraft::Event*) = nullptr;

        InputHandler(GLFWwindow* Window);

        ~InputHandler();

        virtual void PollEvents();

        virtual void HandleEvents(); // Does not clear queue if no callback is set

    private:
        GLFWwindow* Window;
        

    };
}
