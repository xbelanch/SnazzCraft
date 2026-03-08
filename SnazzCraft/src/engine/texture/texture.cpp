#include "snazzcraft-engine/texture/texture.hpp"

SnazzCraft::Texture* SnazzCraft::VoxelTextureAtlas = nullptr;

SnazzCraft::Texture::Texture()
{
    this->ID = new unsigned int;
}

SnazzCraft::Texture::Texture(const char* FilePath)
{
    this->ID = new unsigned int;

    this->LoadTexture(FilePath);
}

SnazzCraft::Texture::~Texture()
{
    this->DeleteBoundData();
    
    delete this->ID;
}

void SnazzCraft::Texture::LoadTexture(const char* FilePath)
{
    this->DeleteBoundData();

    this->Data = stbi_load(FilePath, &this->Dimensions[0], &this->Dimensions[1], &this->NRChannels, 0);
    this->SetTexture();
}

void SnazzCraft::Texture::CreateTextureFromString(std::string Text, unsigned char R, unsigned char G, unsigned char B)
{
    this->DeleteBoundData();

    this->Dimensions[0] = Text.length() * SNAZZCRAFT_CHARACTER_BITWISE_WIDTH;
    this->Dimensions[1] = SNAZZCRAFT_CHARACTER_BITWISE_HEIGHT;

    const unsigned char Alpha = 255;
    this->Data = new unsigned char[this->Dimensions[0] * SNAZZCRAFT_CHARACTER_BITWISE_HEIGHT * NRChannels];

    auto WriteCharData = [this, R, G, B, Alpha](std::string Text, unsigned int CharIndex) -> void
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
        for (unsigned int Y = 0; Y < SNAZZCRAFT_CHARACTER_BITWISE_HEIGHT; Y++) {
            std::string Line;
            std::getline(CharacterFile, Line);
            
            for (unsigned int X = 0; X < SNAZZCRAFT_CHARACTER_BITWISE_WIDTH; X++) {
                unsigned int Index = (Y * this->Dimensions[0] + (CharIndex * SNAZZCRAFT_CHARACTER_BITWISE_WIDTH + X)) * NRChannels;

                this->Data[Index + 0] = CharacterFile.is_open() && Line[X] == '1' && this->NRChannels >= 1 ? R     : 0;
                this->Data[Index + 1] = CharacterFile.is_open() && Line[X] == '1' && this->NRChannels >= 2 ? G     : 0;
                this->Data[Index + 2] = CharacterFile.is_open() && Line[X] == '1' && this->NRChannels >= 3 ? B     : 0;
                this->Data[Index + 3] = CharacterFile.is_open() && Line[X] == '1' && this->NRChannels >= 4 ? Alpha : 0;
            }
        }
    };

    for (unsigned int I = 0; I < Text.length(); I++) { WriteCharData(Text, I); }

    this->SetTexture();
}

bool SnazzCraft::Texture::OverlayNewTexture(const char* FilePath, int OffsetX, int OffsetY)
{
    int NewWidth, NewHeight, NewNRChannels = 0;

    unsigned char* NewData = stbi_load(FilePath, &NewWidth, &NewHeight, &NewNRChannels, 0);
    if (NewData == nullptr || NewNRChannels != this->NRChannels) return false;

    for (int Y = OffsetY; Y < OffsetY + NewHeight; Y++) {
        if (Y < 0 || Y >= this->Dimensions[1]) continue;

        for (int X = OffsetX; X < OffsetX + NewWidth; X++) {
            if (X < 0 || X >= this->Dimensions[0]) continue;

            unsigned int Index = (Y * this->Dimensions[0] + X) * this->NRChannels;
            unsigned int NewDataIndex = ((Y - OffsetY) * NewWidth + (X - OffsetX)) * NewNRChannels;

            for (unsigned int I = 0; I < NewNRChannels; I++) {
                this->Data[Index + I] = NewData[NewDataIndex + I];
            }
        }
    }

    glDeleteTextures(1, this->ID);
    this->SetTexture();

    return true;
}

void SnazzCraft::Texture::SetTexture()
{
    glGenTextures(1, &*this->ID);
    glBindTexture(GL_TEXTURE_2D, *this->ID);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (this->Data == nullptr) return;

    GLenum Format = GL_RGB;

    switch (this->NRChannels) {
    case 1:
        Format = GL_RED;
        break;
    case 3:
        Format = GL_RGB;
        break;
    case 4:
        Format = GL_RGBA;
        break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, Format, this->Dimensions[0], this->Dimensions[1], 0, Format, GL_UNSIGNED_BYTE, this->Data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

