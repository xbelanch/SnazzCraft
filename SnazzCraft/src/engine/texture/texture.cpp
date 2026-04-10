#include "snazzcraft-engine/texture/texture.hpp"

SnazzCraft::Texture* SnazzCraft::VoxelTextureAtlas = nullptr;

SnazzCraft::Texture::Texture(std::string FilePath)
{
    this->ID = new uint32_t;

    this->LoadTexture(FilePath);
}

SnazzCraft::Texture::~Texture()
{
    this->DeleteBoundData();
    
    delete this->ID;
}

void SnazzCraft::Texture::LoadTexture(std::string FilePath)
{
    this->DeleteBoundData();

    this->Data = stbi_load(FilePath.c_str(), &this->Dimensions[0], &this->Dimensions[1], &this->NRChannels, 0);
    this->SetTexture();
}

SnazzCraft::Texture::Texture()
{
    
}

void SnazzCraft::Texture::SetTexture()
{
    if (this->ID == nullptr) return;

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

