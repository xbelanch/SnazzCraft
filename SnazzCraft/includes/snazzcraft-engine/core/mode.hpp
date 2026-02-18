#pragma once

#include "../input-handler/event.hpp"

#define SNAZZCRAFT_USER_MODE_WORLD      (0x00)
#define SNAZZCRAFT_USER_MODE_MAIN_MENU  (0x01)

namespace SnazzCraft
{
    extern unsigned char UserMode;

    extern bool WireframeModeActive;
}

