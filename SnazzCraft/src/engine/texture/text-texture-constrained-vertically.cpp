#include "snazzcraft-engine/texture/text-texture-constrained-vertically.hpp"


SnazzCraft::TextTextureConstrainedVertically::TextTextureConstrainedVertically(std::string IText, uint8_t IR, uint8_t IG, uint8_t IB)
    : TextTexture(IText, IR, IG, IB)
{
    this->Update();

    
}

void SnazzCraft::TextTextureConstrainedVertically::Update()
{
    this->DeleteBoundData();

    this->Width  = Text.length() * SnazzCraft::TextTexture::PerCharacterWidth;
    this->Height = SnazzCraft::TextTexture::PerCharacterHeight;

    this->Data = new uint8_t[this->Width * this->Height * static_cast<uint32_t>(this->NRChannels)];

    auto WriteCharData = [this](std::string Text, uint32_t CharIndex) -> void
    {
        char Character = std::tolower(Text[CharIndex]);

        const std::string FilePath = [&]() -> std::string
        {
            switch (Character) // Special cases
            {
                case ' ':
                    return "textures/gui/font/space.txt";
                    break;

                default:
                    return std::string("textures/gui/font/") + Character + ".txt";
                    break;
            }
        }();
        

        std::ifstream CharacterFile(FilePath);
        for (uint32_t Y = 0; Y < SnazzCraft::TextTexture::PerCharacterHeight; Y++) {
            std::string Line;
            std::getline(CharacterFile, Line);
            
            for (uint32_t X = 0; X < SnazzCraft::TextTexture::PerCharacterWidth; X++) {
                uint32_t Index = (Y * this->Dimensions[0] + (CharIndex * SnazzCraft::TextTexture::PerCharacterWidth + X)) * NRChannels;

                this->Data[Index + 0] = CharacterFile.is_open() && Line[X] == '1' && this->NRChannels >= 1 ? this->R     : 0;
                this->Data[Index + 1] = CharacterFile.is_open() && Line[X] == '1' && this->NRChannels >= 2 ? this->G     : 0;
                this->Data[Index + 2] = CharacterFile.is_open() && Line[X] == '1' && this->NRChannels >= 3 ? this->B     : 0;
                this->Data[Index + 3] = CharacterFile.is_open() && Line[X] == '1' && this->NRChannels >= 4 ? this->Alpha : 0;
            }
        }
    };

    for (uint32_t I = 0; I < Text.length(); I++) { WriteCharData(Text, I); }

    this->ID = new uint32_t;
    this->SetTexture();
}

