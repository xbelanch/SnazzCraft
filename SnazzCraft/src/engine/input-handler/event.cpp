#include "snazzcraft-engine/input-handler/event.hpp"

SnazzCraft::Event::Event(unsigned char Type)
{
    this->Type = Type;
}

SnazzCraft::Event::~Event()
{
    delete this->EventData;
}


