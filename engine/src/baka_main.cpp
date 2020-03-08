#include "Baka.h"
#include "baka_graphics.h"
#include "baka_input.h"
#include "SDL.h"
#include "baka_logger.h"

int main(int argc, char *argv[])
{
    bool running = false;
    running = Baka::Graphics::Init();
    Baka::Input::Init();
    
    BakaMain(argc, argv);

    while(running)
    {
        Baka::Input::Update();

        if( Baka::Input::QuitRequested() || Baka::Input::KeyPressed(SDL_SCANCODE_ESCAPE) )
        {
            running = false;
        }

        BakaUpdate(0.0f);
    }

    return 0;
}