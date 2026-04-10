#include "snazzcraft-engine/gui/main-menu-gui.hpp"
#include "snazzcraft-engine/gui/panel/panel-with-texture.hpp"

SnazzCraft::MainMenuGUI::MainMenuGUI()
    : GUI()
{
    this->Panels.push_back(new SnazzCraft::PanelWithTexture(0u, 0u, 900u, 900u, "textures/voxel/individuals/dirt.png"));
}


