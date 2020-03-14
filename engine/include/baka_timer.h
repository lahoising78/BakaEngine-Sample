#ifndef _BAKA_TIMER_H_
#define _BAKA_TIMER_H_

#include <cstdint>

namespace baka
{
    class Timer
    {
    public:
        Timer();
        void Start();
        float GetTicks();
        void Pause();
        void Resume();

    private:
        uint32_t start_time;
        uint32_t pause_time;
        bool paused;
        bool started;
    };
}

#endif