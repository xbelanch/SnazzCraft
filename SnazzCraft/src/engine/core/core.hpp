#pragma once

#include <vector>

#include "../../../includes/glad.h"
#include "../../../includes/glfw3.h"
#include "../../../includes/shader_s.h"
#include "../../../includes/stb_image.h"

#include "../../../includes/glm/glm.hpp"
#include "../../../includes/glm/gtc/matrix_transform.hpp"
#include "../../../includes/glm/gtc/type_ptr.hpp"

#include "../mesh/mesh.hpp"
#include "../texture/texture.hpp"
#include "../world/voxel/voxel.hpp"
#include "../entity/entity.hpp"
#include "../world/chunk/chunk.hpp"
#include "../world/world.hpp"
#include "gui/gui.hpp"
#include "input-handler/input-handler.hpp"
#include "input-handler/event/events.h"
#include "mode/mode.hpp"
#include "../entity/user/user.hpp"
#include "input-handler/event/data/data-types.h"
#include "../fps-tracker/fps-tracker.hpp"
#include "gui/main-menu-gui/main-menu-gui.hpp"
#include "gui/world-gui/world-gui.hpp"
#include "window/window.hpp"
#include "../hitbox/hitbox.hpp"

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
}

inline void FrameBufferSizeCallBack(GLFWwindow* Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);

    SnazzCraft::MenuGUI->Resize(Width, Height);
    SnazzCraft::WorldGUI->Resize(Width, Height);
}

inline void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods)
{
    if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS) {
        double MouseX, MouseY;
        glfwGetCursorPos(Window, &MouseX, &MouseY);

        SnazzCraft::Event* NewEvent = new SnazzCraft::Event(SNAZZCRAFT_EVENT_MOUSE_CLICK_LEFT);
        NewEvent->EventData->Items.push_back(static_cast<void*>(new glm::dvec2(MouseX, MouseY)));
        NewEvent->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_DVEC2);

        switch (SnazzCraft::UserMode) {
            case SNAZZCRAFT_USER_MODE_WORLD:
                NewEvent->EventData->Items.push_back(static_cast<void*>(SnazzCraft::WorldGUI));
                NewEvent->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_GUI_ADDRESS);

                SnazzCraft::WorldGUI->GUIInputHandler->EventQueue.push_back(NewEvent);

                break;

            case SNAZZCRAFT_USER_MODE_MAIN_MENU:
                NewEvent->EventData->Items.push_back(static_cast<void*>(SnazzCraft::MenuGUI));
                NewEvent->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_GUI_ADDRESS);

                SnazzCraft::MenuGUI->GUIInputHandler->EventQueue.push_back(NewEvent); 

                break;
        }
    }
}
