#include "baka_input.h"
#include "baka_logger.h"

namespace baka
{
    SDL_KeyboardEvent Input::keyboard[ BAKA_NUM_KEYS ];
    SDL_QuitEvent Input::quitEvent;
    SDL_TextInputEvent Input::lastKey;

    void Input::Init()
    {
        memset(keyboard, 0, sizeof(SDL_KeyboardEvent) * BAKA_NUM_KEYS);
    }

    void Input::Update()
    {
        SDL_Event e = {0};
        memset(keyboard, 0, sizeof(SDL_KeyboardEvent) * BAKA_NUM_KEYS);
        quitEvent = {0};
        lastKey = {0};

        while(SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_KEYUP:
            case SDL_KEYDOWN:
                if(e.key.keysym.scancode < BAKA_NUM_KEYS)
                {
                    keyboard[ e.key.keysym.scancode ] = e.key;
                }
                break;

            case SDL_TEXTINPUT:
                lastKey = e.text;
                break;

            case SDL_QUIT:
                quitEvent = e.quit;
                break;
            
            default:
                break;
            }
        }
    }

    bool Input::KeyPressed(int key)
    {
        if(key >= BAKA_NUM_KEYS) return false;
        return keyboard[key].state == SDL_PRESSED;
    }

    const char *Input::AnyKey()
    {
        return lastKey.text;
    }

    bool Input::QuitRequested()
    {
        return quitEvent.type == SDL_QUIT;
    }
}