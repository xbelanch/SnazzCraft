#include "snazzcraft-engine/gui/widget-collection.hpp"

SnazzCraft::WidgetCollection::~WidgetCollection()
{
    for (SnazzCraft::Panel* Panel : this->Panels) {
        delete Panel;
    }

    for (SnazzCraft::Button* Button : this->Buttons) {
        delete Button;
    }
}

void SnazzCraft::WidgetCollection::Draw()
{
    for (SnazzCraft::Panel* Panel : this->Panels) {
        Panel->Draw();
    }

    for (SnazzCraft::Button* Button : this->Buttons) {
        Button->Draw();
    }
}
