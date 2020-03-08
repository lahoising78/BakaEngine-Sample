#include "baka_graphics.h"
#include "baka_logger.h"

namespace Baka
{
    SDL_Window *Graphics::window = nullptr;

    bool Graphics::Init()
    {
        if(SDL_Init( SDL_INIT_EVERYTHING ) != 0)
        {
            bakaerr("Failed to initialize SDL");
            return false;
        }
        atexit(SDL_Quit);

        Graphics::Setup();

        atexit(Graphics::Close);

        bakalog("baka graphics initialized");
        return true;
    }

    void Graphics::Setup()
    {
        uint32_t windowFlags = SDL_WINDOW_VULKAN;

        window = SDL_CreateWindow(
            "Baka Engine",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1280, 720,
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