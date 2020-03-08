#include "Baka.h"
#include "baka_graphics.h"
#include "SDL.h"
#include "baka_logger.h"

int main(int argc, char *argv[])
{
    SDL_Event e = {0};
    bool running = false;
    running = Baka::Graphics::Init();
    
    BakaMain(argc, argv);

    while(running)
    {
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT)
            {
                running = false;
            }
        }
    }

    return 0;
}