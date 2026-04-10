#pragma once

#include <vector>
#include <stdint.h>

#include "glad.h"
#include "glfw3.h"
#include "shader_s.h"
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "snazzcraft-engine/gui/main-menu-gui.hpp"
#include "snazzcraft-engine/input-handler/events.h"
#include "snazzcraft-engine/input-handler/event.hpp"
#include "snazzcraft-engine/input-handler/data-types.h"
#include "snazzcraft-engine/core/mode.hpp"
#include "snazzcraft-engine/input-handler/input-handler.hpp"

#ifdef _WIN32
    #define CLEAR_COMMAND "cls"
#endif // _WIN32

#ifdef __linux__
    #define CLEAR_COMMAND "clear"
#endif // __linux__

namespace SnazzCraft
{
    extern Shader* VoxelShader;

    extern glm::mat4 ProjectionMatrix;
    extern int ProjectionLock;

    extern glm::mat4 ModelMatrix;
    extern int ModelLock;

    extern glm::mat4 ViewMatrix;
    extern int ViewLock;

    extern bool CloseApplication;

    bool Initiate();

    void MainLoop();

    void FreeResources();
} // SnazzCraft

inline void FrameBufferSizeCallBack(GLFWwindow* Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);
}

inline void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods)
{
    double MouseX, MouseY;
    glfwGetCursorPos(Window, &MouseX, &MouseY);

    unsigned char EventType;
    if (Button == GLFW_MOUSE_BUTTON_LEFT) {
        EventType = (Action == GLFW_PRESS) ? SNAZZCRAFT_EVENT_MOUSE_CLICK_LEFT_PRESS : SNAZZCRAFT_EVENT_MOUSE_CLICK_LEFT_RELEASE;
    } else if (Button == GLFW_MOUSE_BUTTON_RIGHT) {
        EventType = (Action == GLFW_PRESS) ? SNAZZCRAFT_EVENT_MOUSE_CLICK_RIGHT_PRESS : SNAZZCRAFT_EVENT_MOUSE_CLICK_RIGHT_RELEASE;
    } else {
        return;
    }

    SnazzCraft::Event* NewEvent = new SnazzCraft::Event(EventType);
    NewEvent->EventData->Items.push_back(static_cast<void*>(new glm::dvec2(MouseX, MouseY)));
    NewEvent->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_DVEC2);

    switch (SnazzCraft::UserMode) {
        case SNAZZCRAFT_USER_MODE_WORLD:
            //NewEvent->EventData->Items.push_back(static_cast<void*>(SnazzCraft::WorldGUI));
            //NewEvent->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_GUI_ADDRESS);

            //SnazzCraft::WorldGUI->GUIInputHandler->EventQueue.push_back(NewEvent);

            break;

        case SNAZZCRAFT_USER_MODE_MAIN_MENU:
            //NewEvent->EventData->Items.push_back(static_cast<void*>(SnazzCraft::MenuGUI));
            //NewEvent->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_GUI_ADDRESS);

            //SnazzCraft::MenuGUI->GUIInputHandler->EventQueue.push_back(NewEvent); 

            break;
    }
}
