#include "snazzcraft-engine/texture/solid-colors.hpp"
#include "snazzcraft-engine/texture/texture.hpp"

const char* SolidColorDirectory = "textures/solids/";
const char* SolidColorExtension = ".png";

SnazzCraft::Texture* SnazzCraft::GenerateSolidColorTexture(std::string ColorName)
{
    SnazzCraft::Texture* SolidColorTexture = new SnazzCraft::Texture(std::string(SolidColorDirectory + ColorName + SolidColorExtension).c_str());

    return SolidColorTexture;
}


