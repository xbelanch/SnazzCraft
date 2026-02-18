#include "snazzcraft-engine/gui/world-gui.hpp"

SnazzCraft::InWorldGUI* SnazzCraft::WorldGUI = nullptr;

SnazzCraft::InWorldGUI::InWorldGUI(unsigned int WindowWidth, unsigned int WindowHeight, GLFWwindow* Window) : SnazzCraft::GUI(WindowWidth, WindowHeight)
{
    this->SetInputHandler(new SnazzCraft::InputHandler(Window));

    this->WidgetCollections.push_back(new SnazzCraft::WidgetCollection());
    this->ActiveWidgetCollectionIndex = 1;

    this->WidgetCollections[1]->Panels.push_back(SNAZZCRAFT_PANEL_LONG(10, 10, new SnazzCraft::Texture("textures/gui/button.png"))); 

    this->WidgetCollections[1]->Buttons.push_back(SNAZZCRAFT_BUTTON_LONG(10, 10));
    this->WidgetCollections[1]->Buttons[0]->SwapPanels(new SnazzCraft::Texture());
    this->WidgetCollections[1]->Buttons[0]->ActivePanel->Texture->CreateTextureFromString("activiate wireframe", 255, 255, 255);
    this->WidgetCollections[1]->Buttons[0]->Callback = &ToggleWireframeCallback;
}
