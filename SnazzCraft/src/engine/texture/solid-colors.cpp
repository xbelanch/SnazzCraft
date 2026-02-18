#include "snazzcraft-engine/texture/solid-colors.hpp"

const char* SolidColorDirectory = "textures/solids/";
const char* SolidColorExtension = ".png";

SnazzCraft::Texture* SnazzCraft::GenerateSolidColorTexture(std::string ColorName)
{
    SnazzCraft::Texture* SolidColorTexture = new SnazzCraft::Texture();
    SolidColorTexture->LoadTexture(std::string(SolidColorDirectory + ColorName + SolidColorExtension).c_str());

    return SolidColorTexture;
}


