#pragma once

#include <vector>

#include "glad.h"
#include "snazzcraft-engine/input-handler/input-handler.hpp"

#include "snazzcraft-engine/gui/panel/panel.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader;
class GLFWwindow;
namespace SnazzCraft
{
    class GUI
    {
    public:
        GUI();

        ~GUI();

        virtual void Resize(float Width, float Height);

        inline void Draw() const
        {
            this->ProtectedDraw();
        }

        inline void AddPanel(SnazzCraft::Panel* NewPanel)
        {
            this->Panels.push_back(NewPanel);
        }

        inline void SetInputHandler(GLFWwindow* Window) 
        {
            this->InputHandler = SnazzCraft::InputHandler(Window);
        }

        inline void SetInputHandlerCallback(void(*NewCallback)(SnazzCraft::Event* Event))
        {
            this->InputHandler.Callback = NewCallback;
        }

        inline void PollEvents() 
        {
            this->InputHandler.PollEvents();
        }

        inline void HandleEvents()
        {
            this->InputHandler.HandleEvents();
        }

        inline void AddEvent(SnazzCraft::Event* Event) 
        {
            this->InputHandler.EventQueue.push_back(Event);
        }

        inline void SendEventToPanels(SnazzCraft::Event* Event) const
        {
            for (SnazzCraft::Panel* Panel : this->Panels) {
                Panel->HandleEvent(Event);
            }
        }

    protected:
        std::vector<SnazzCraft::Panel*>  Panels;

        Shader* GUIShader;
        glm::mat4 OrthographicProjectionMatrix;
        int OrthographicLock;
        
        SnazzCraft::InputHandler InputHandler;

        virtual void ProtectedDraw() const;

    private:


    };
} // SnazzCraft
