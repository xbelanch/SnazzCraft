#pragma once

#include "gui.hpp"
#include "../input-handler/core-callback.hpp"

namespace SnazzCraft
{
    class InWorldGUI : public SnazzCraft::GUI
    {
    public:

        InWorldGUI(unsigned int WindowWidth, unsigned int WindowHeight, GLFWwindow* Window);

        bool InMenu = false;

    private:


    };


    extern SnazzCraft::InWorldGUI* WorldGUI;
}

