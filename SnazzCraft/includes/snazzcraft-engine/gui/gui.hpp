#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader;
namespace SnazzCraft
{
    class Panel;
    class Button;

    class GUI
    {
    public:
        GUI();

        ~GUI();

        virtual void Resize(uint32_t Width, uint32_t Height);

        inline void Draw() const
        {
            this->ProtectedDraw();
        }

        inline void AddPanel(SnazzCraft::Panel* NewPanel)
        {
            this->Panels.push_back(NewPanel);
        }

        inline void AddButton(SnazzCraft::Button* NewButton)
        {
            this->Buttons.push_back(NewButton);
        }

    protected:
        std::vector<SnazzCraft::Panel*>  Panels;
        std::vector<SnazzCraft::Button*> Buttons;

        Shader* GUIShader;
        glm::mat4 OrthographicProjectionMatrix;
        int OrthographicLock;

    private:
        virtual void ProtectedDraw() const; // Panels are drawn first

    };
} // SnazzCraft