#include "snazzcraft-engine/gui/gui.hpp"
#include "snazzcraft-engine/gui/panel/panel.hpp"
#include "snazzcraft-engine/gui/button/button.hpp"

#include "shader_s.h"
#include "glad.h"

constexpr const char* GUIVertexShaderFilePath   = "src/engine/shaders/gui/vertex-shader.glsl";
constexpr const char* GUIFragmentShaderFilePath = "src/engine/shaders/gui/fragment-shader.glsl";

SnazzCraft::GUI::GUI()
{
    this->GUIShader = new Shader(GUIVertexShaderFilePath, GUIFragmentShaderFilePath);
    this->GUIShader->use();
    
    this->OrthographicLock = glGetUniformLocation(this->GUIShader->ID, "OrthographicProjection");

    this->Resize(900u, 900u);
}

SnazzCraft::GUI::~GUI()
{
    for (SnazzCraft::Panel* Panel : this->Panels) {
        delete Panel;
    }

    for (SnazzCraft::Button* Button : this->Buttons) {
        delete Button;
    }

}

void SnazzCraft::GUI::Resize(uint32_t Width, uint32_t Height)
{
    this->GUIShader->use();
    this->OrthographicProjectionMatrix = glm::ortho(0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, -1.0f, 1.0f);  
    glUniformMatrix4fv(this->OrthographicLock, 1, GL_FALSE, glm::value_ptr(this->OrthographicProjectionMatrix));
}

void SnazzCraft::GUI::ProtectedDraw() const
{
    glDisable(GL_DEPTH_TEST);

    for (SnazzCraft::Panel* Panel : this->Panels) {
        Panel->Draw();
    }

    for (SnazzCraft::Button* Button : this->Buttons) {
        Button->Draw();
    }
}

