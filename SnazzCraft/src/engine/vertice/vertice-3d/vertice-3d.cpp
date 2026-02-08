#include "vertice-3d.hpp"

SnazzCraft::Vertice3D::Vertice3D()
{

}

SnazzCraft::Vertice3D::Vertice3D(glm::vec3 Position, glm::vec2 TextureCoordinate) 
{ 
    this->Position = Position; 
    this->TextureCoordinate = TextureCoordinate; 
}

SnazzCraft::Vertice3D::Vertice3D(glm::vec3 Position, glm::vec2 TextureCoordinate, float Brightness)
{
    this->Position = Position;
    this->TextureCoordinate = TextureCoordinate;
    this->Brightness = Brightness;
}