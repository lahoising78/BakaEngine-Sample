#include "baka_graphics.h"
#include "baka_logger.h"

namespace Baka
{
    SDL_Window *Graphics::window = nullptr;

    bool Graphics::Init( const char *windowName, int width, int height )
    {
        if(SDL_Init( SDL_INIT_EVERYTHING ) != 0)
        {
            bakaerr("Failed to initialize SDL");
            return false;
        }
        atexit(SDL_Quit);

        Graphics::Setup(windowName, width, height);

        atexit(Graphics::Close);

        bakalog("baka graphics initialized");
        return true;
    }

    void Graphics::Setup( const char *windowName, int width, int height )
    {
        uint32_t windowFlags = SDL_WINDOW_VULKAN;

        window = SDL_CreateWindow(
            windowName,
            0, 0,
            width, height,
            windowFlags
        );
    }

    void Graphics::Close()
    {
        bakalog("closing baka graphics");
        if(window)
        {
            SDL_DestroyWindow( window );
        }
    }
}