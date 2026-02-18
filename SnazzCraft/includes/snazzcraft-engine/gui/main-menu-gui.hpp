#pragma once

#include "gui.hpp"
#include "../input-handler/input-handler.hpp"
#include "../core/window.hpp"

namespace SnazzCraft
{
    class MainMenuGUI : public SnazzCraft::GUI
    {
    public:


        MainMenuGUI(unsigned int WindowWidth, unsigned int WindowHeight, GLFWwindow* Window);

        


    private:


    };

    extern SnazzCraft::MainMenuGUI* MenuGUI;
}




