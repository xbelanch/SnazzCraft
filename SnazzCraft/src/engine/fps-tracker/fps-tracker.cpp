#include "snazzcraft-engine/fps-tracker/fps-tracker.hpp"

SnazzCraft::FPSTracker* SnazzCraft::GlobalFPSTracker = new SnazzCraft::FPSTracker();

SnazzCraft::FPSTracker::FPSTracker()
{
    this->LastTime = glfwGetTime();
}

void SnazzCraft::FPSTracker::UpdateFPS()
{
    this->FrameCount++;
    this->CurrentTime = glfwGetTime();
    
    if (this->CurrentTime - this->LastTime >= 1.0) {
        this->FPS = static_cast<double>(this->FrameCount) / (this->CurrentTime - this->LastTime);
        this->FrameCount = 0;
        this->LastTime = this->CurrentTime;
    }
}