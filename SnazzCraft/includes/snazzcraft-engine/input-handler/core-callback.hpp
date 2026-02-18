#pragma once

#include "../../glad.h"
#include "../../glfw3.h"

#include "../core/mode.hpp"
#include "../world/world.hpp"

namespace SnazzCraft
{
    void ChangeModeCallback(SnazzCraft::Event* Event);

    void CloseWindowCallback(SnazzCraft::Event* Event);

    void ToggleWireframeCallback(SnazzCraft::Event* Event);
}


