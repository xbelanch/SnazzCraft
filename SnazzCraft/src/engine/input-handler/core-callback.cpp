#include "snazzcraft-engine/input-handler/core-callback.hpp"

void SnazzCraft::ChangeModeCallback(SnazzCraft::Event* Event)
{
    unsigned char* Mode = static_cast<unsigned char*>(Event->EventData->AccessDataType(SNAZZCRAFT_DATA_TYPE_SWITCH_MODE));
    if (Mode == nullptr) return;
 
    switch (*Mode) 
    {
        case SNAZZCRAFT_USER_MODE_WORLD:
        {
            SnazzCraft::UserMode = SNAZZCRAFT_USER_MODE_WORLD;

            SnazzCraft::World* LoadedWorld = static_cast<SnazzCraft::World*>(Event->EventData->AccessDataType(SNAZZCRAFT_DATA_TYPE_WORLD_ADDRESS));
            if (LoadedWorld == nullptr) return;

            SnazzCraft::CurrentWorld = LoadedWorld;

            break;
        }
            
        case SNAZZCRAFT_USER_MODE_MAIN_MENU:
        {
            SnazzCraft::UserMode = SNAZZCRAFT_USER_MODE_MAIN_MENU;
            break;
        }  
    }
}

void SnazzCraft::CloseWindowCallback(SnazzCraft::Event* Event)
{
    void* Window = Event->EventData->AccessDataType(SNAZZCRAFT_DATA_TYPE_GLFW_WINDOW);
    if (Window == nullptr) return;

    glfwSetWindowShouldClose(static_cast<GLFWwindow*>(Window), GLFW_TRUE);
}

void SnazzCraft::ToggleWireframeCallback(SnazzCraft::Event* Event)
{
    SnazzCraft::WireframeModeActive = !SnazzCraft::WireframeModeActive;
}