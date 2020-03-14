#include <SDL2/SDL.h>
#include "baka_timer.h"

namespace baka
{
    Timer::Timer()
    {
        start_time = 0;
        pause_time = 0;
        started = false;
        paused = false;
    }

    float Timer::GetTicks()
    {
        if(!started)
        {
            return 0.0f;
        }

        if(paused)
        {
            return (float)(pause_time - start_time)/1000.0f;
        }

        return (float)(SDL_GetTicks() - start_time)/1000.0f;
    }

    void Timer::Pause()
    {
        paused = true;
        pause_time = SDL_GetTicks();
    }

    void Timer::Resume()
    {
        pause_time = 0;
        paused = false;
    }

    void Timer::Start()
    {
        start_time = SDL_GetTicks();
        started = true;
    }

}