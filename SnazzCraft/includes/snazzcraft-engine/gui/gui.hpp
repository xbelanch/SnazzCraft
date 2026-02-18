#pragma once

#include <vector>

#include "../../glad.h"
#include "../../shader_s.h"
#include "../../glm/glm.hpp"
#include "../../glm/gtc/matrix_transform.hpp"
#include "../../glm/gtc/type_ptr.hpp"

#include "button.hpp"
#include "../input-handler/input-handler.hpp"
#include "../input-handler/events.h"
#include "widget-collection.hpp"
#include "../input-handler/core-callback.hpp"
#include "../texture/texture.hpp"
#include "../world/world.hpp"

namespace SnazzCraft
{
    class GUI
    {
    public:
        std::vector<SnazzCraft::WidgetCollection*> WidgetCollections = { nullptr }; // Index 0 is always nullptr
        unsigned char ActiveWidgetCollectionIndex = 0;

        Shader* GUIShader = nullptr;

        glm::mat4 OrthographicProjectionMatrix;
        int OrthographicLock;

        SnazzCraft::InputHandler* GUIInputHandler = nullptr;

        GUI(unsigned int WindowWidth, unsigned int WindowHeight);

        virtual ~GUI();

        void Render(); // Panels are drawn before buttons

        virtual void SendEventToButtons(SnazzCraft::Event* Event);

        virtual inline void Resize(unsigned int WindowWidth, unsigned int WindowHeight)
        {
            this->GUIShader->use();

            this->OrthographicProjectionMatrix = glm::ortho(0.0f, static_cast<float>(WindowWidth), static_cast<float>(WindowHeight), 0.0f, -1.0f, 1.0f);  
            glUniformMatrix4fv(this->OrthographicLock, 1, GL_FALSE, glm::value_ptr(this->OrthographicProjectionMatrix));
        }

        inline void SetInputHandler(SnazzCraft::InputHandler* NewInputHandler)
        {
            delete this->GUIInputHandler;
            this->GUIInputHandler = NewInputHandler;
        }

    private:


    };
}