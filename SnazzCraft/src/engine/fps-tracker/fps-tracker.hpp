#pragma once

#include "../../../includes/glfw3.h"

namespace SnazzCraft
{
    class FPSTracker
    {
    public:
        unsigned int FrameCount = 0;
        double FPS = 0.0;

        FPSTracker();

        void UpdateFPS();

        inline double GetDeltaTime()
        {
            return CurrentTime - LastTime;
        }

    private:
        double LastTime = 0.0;
        double CurrentTime = 0.0;

    };

    extern SnazzCraft::FPSTracker* GlobalFPSTracker;
}