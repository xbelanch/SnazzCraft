#include "gui.hpp"

const char* GUIVertexShaderFilePath   = "src/engine/shaders/gui/vertex-shader.glsl";
const char* GUIFragmentShaderFilePath = "src/engine/shaders/gui/fragment-shader.glsl";

SnazzCraft::GUI::GUI(unsigned int WindowWidth, unsigned int WindowHeight)
{
    this->GUIShader = new Shader(GUIVertexShaderFilePath, GUIFragmentShaderFilePath);
    this->GUIShader->use();
    
    this->OrthographicLock = glGetUniformLocation(this->GUIShader->ID, "OrthographicProjection");

    this->Resize(WindowWidth, WindowHeight);
}

SnazzCraft::GUI::~GUI()
{
    delete this->GUIShader;
    delete this->GUIInputHandler;
    
    for (SnazzCraft::WidgetCollection* Collection : this->WidgetCollections) {
        delete Collection;
    }
}

void SnazzCraft::GUI::Render()
{
    this->GUIShader->use();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    this->WidgetCollections[this->ActiveWidgetCollectionIndex]->Draw();
}

void SnazzCraft::GUI::SendEventToButtons(SnazzCraft::Event* Event)
{
    if (this->WidgetCollections[this->ActiveWidgetCollectionIndex] == nullptr) return;
   
    for (SnazzCraft::Button* Button : this->WidgetCollections[this->ActiveWidgetCollectionIndex]->Buttons) {
        Button->HandleClick(Event); 
    }
}

