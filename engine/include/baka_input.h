#ifndef _BAKA_INPUT_H_
#define _BAKA_INPUT_H_

#include <SDL2/SDL.h>

#define BAKA_NUM_KEYS SDL_NUM_SCANCODES

namespace Baka
{
    class Input
    {
    public:
        static void Init();
        static void Update();

        static bool KeyPressed(int key);
        static const char *AnyKey();
        static bool QuitRequested();

    private:
        static SDL_KeyboardEvent keyboard[ BAKA_NUM_KEYS ];
        static SDL_QuitEvent quitEvent;
        static SDL_TextInputEvent lastKey;
    };
}

#endif