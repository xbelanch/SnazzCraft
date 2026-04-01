#include "snazzcraft-engine/gui/button.hpp"

SnazzCraft::Button::Button(unsigned int X, unsigned int Y, unsigned int Width, unsigned int Height)
{
    this->Position[0] = X;
    this->Position[1] = Y;
    this->Dimensions[0] = Width;
    this->Dimensions[1] = Height;
}

SnazzCraft::Button::~Button()
{
    delete this->ActivePanel;
}

void SnazzCraft::Button::Draw()
{
    if (this->ActivePanel == nullptr) return;

    this->ActivePanel->Draw();
}

bool SnazzCraft::Button::HandleClick(SnazzCraft::Event* Event)
{
    if (this->Callback == nullptr) return false;

    glm::dvec2* MousePosition = static_cast<glm::dvec2*>(Event->EventData->AccessDataType(SNAZZCRAFT_DATA_TYPE_DVEC2));
    if (MousePosition == nullptr) return false;

    if (!this->WithinButton(MousePosition->x, MousePosition->y)) return false;

    this->Callback(Event);

    return true;
}

void SetWorldModeButtonCallback(SnazzCraft::Event* Event)
{
    Event->EventData->Items.push_back(static_cast<void*>(new unsigned char(SNAZZCRAFT_USER_MODE_WORLD)));
    Event->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_SWITCH_MODE);

    SnazzCraft::World* NewWorld = SnazzCraft::CurrentWorld;

    unsigned int NewWorldSize = 4;
    if (NewWorld == nullptr) { 
        NewWorld = SnazzCraft::World::CreateWorld("default-generated-world", NewWorldSize, 58008);
    }

    Event->EventData->Items.push_back(static_cast<void*>(NewWorld));
    Event->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_WORLD_ADDRESS);

    SnazzCraft::ChangeModeCallback(Event);
}

void SetMainMenuModeButtonCallback(SnazzCraft::Event* Event)
{
    Event->EventData->Items.push_back(static_cast<void*>(new unsigned char(SNAZZCRAFT_USER_MODE_MAIN_MENU)));
    Event->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_SWITCH_MODE);

    SnazzCraft::ChangeModeCallback(Event);
}


