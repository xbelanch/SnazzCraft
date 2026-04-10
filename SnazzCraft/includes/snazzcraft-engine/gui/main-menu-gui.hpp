#pragma once

#include "snazzcraft-engine/gui/gui.hpp"

namespace SnazzCraft
{
    class MainMenuGUI : public GUI
    {
    public:
        ~MainMenuGUI() = default;

    protected:
        MainMenuGUI();

    private:


    public:
        static inline const MainMenuGUI& GetInstance()
        {
            static SnazzCraft::MainMenuGUI Instance;
            return Instance;
        }

    };
} // SnazzCraft