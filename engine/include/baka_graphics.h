#ifndef _BAKA_GRAPHICS_H_
#define _BAKA_GRAPHICS_H_

#include <SDL2/SDL.h>

namespace Baka{
    class Graphics
    {
    public:
        static bool Init( const char *windowName, int width, int height );

    private:
        static void Setup( const char *windowName, int width, int height );
        static void Close();

    private:
        static SDL_Window *window;
    };
}

#endif