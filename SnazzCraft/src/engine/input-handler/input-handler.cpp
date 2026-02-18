#include "snazzcraft-engine/input-handler/input-handler.hpp"

SnazzCraft::InputHandler::InputHandler(GLFWwindow* Window)
{
    this->Window = Window;
}

SnazzCraft::InputHandler::~InputHandler()
{
    for (SnazzCraft::Event* Event : this->EventQueue) {
        delete Event;
    }
}

void SnazzCraft::InputHandler::PollEvents()
{
    for (unsigned char I = 0; I < SNAZZCRAFT_KEY_CHECK_AMOUNT; I++) { // I is SnazzCraft key
        if (glfwGetKey(this->Window, SnazzCraftKeyToGLFWKey[I]) == GLFW_PRESS) {
            SnazzCraft::Event* NewEvent = new SnazzCraft::Event(SNAZZCRAFT_EVENT_KEY_DOWN);

            NewEvent->EventData->Items.push_back(static_cast<void*>(new unsigned char(I)));
            NewEvent->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_KEY);

            NewEvent->EventData->Items.push_back(static_cast<void*>(this->Window));
            NewEvent->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_GLFW_WINDOW);

            this->EventQueue.push_back(NewEvent);
        }
    }
}

void SnazzCraft::InputHandler::HandleEvents()
{
    if (this->Callback == nullptr) return;

    for (int I = this->EventQueue.size() - 1; I >= 0; I--) {
        SnazzCraft::Event* CurrentEvent = this->EventQueue[I];

        this->Callback(CurrentEvent);

        delete CurrentEvent;
        this->EventQueue.pop_back();
    }
}
