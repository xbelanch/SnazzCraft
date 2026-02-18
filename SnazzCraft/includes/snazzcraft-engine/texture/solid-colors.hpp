#pragma once

#include <string>

#include "texture.hpp"

#define SNAZZCRAFT_COLOR_BLACK ("black")
#define SNAZZCRAFT_COLOR_DARK_BLUE  ("dark-blue")
#define SNAZZCRAFT_COLOR_DARK_GREEN ("dark-green")
#define SNAZZCRAFT_COLOR_DARK_RED   ("dark-red")
#define SNAZZCRAFT_COLOR_LIGHT_BLUE ("light-blue")
#define SNAZZCRAFT_COLOR_LIGHT_GRAY ("light-gray")
#define SNAZZCRAFT_COLOR_LIGHT_GREEN ("light-green")
#define SNAZZCRAFT_COLOR_LIGHT_RED  ("light-red")
#define SNAZZCRAFT_COLOR_WHITE ("white")

namespace SnazzCraft
{
    SnazzCraft::Texture* GenerateSolidColorTexture(std::string ColorName); // Only give predefined color names
}



