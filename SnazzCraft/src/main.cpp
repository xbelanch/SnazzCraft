#define STB_IMAGE_IMPLEMENTATION

#include "engine/core/core.hpp"

int main(int ArgC, char* ArgV[])
{ 
    if (!SnazzCraft::Initiate())
    {
        return -1;
    }

    if (ArgC == 2) 
    {
        std::string WorldFilePath = "worlds/" + std::string(ArgV[1]) + ".txt";
        SnazzCraft::CurrentWorld = SnazzCraft::World::LoadWorldFromSaveFile(WorldFilePath);
    }

    SnazzCraft::MainLoop();

    SnazzCraft::FreeResources();
    
    return 0;
}