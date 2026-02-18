#pragma once

#include <vector>

#include "panel.hpp"
#include "button.hpp"

namespace SnazzCraft
{
    struct WidgetCollection
    {
    public:
        std::vector<SnazzCraft::Panel*> Panels;
        std::vector<SnazzCraft::Button*> Buttons;

        WidgetCollection() = default;

        ~WidgetCollection();

        void Draw(); // Panels are drawn before buttons

    private:

    };
}


