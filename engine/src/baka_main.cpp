#include "Baka.h"
#include "baka_graphics.h"
#include "baka_input.h"
#include "SDL.h"
#include "baka_logger.h"
#include "baka_timer.h"

int main(int argc, char *argv[])
{
    bool running = false;
    bool validation = true;
    uint32_t fps = 0;
    baka::Timer frames;
    bakalog("--==== Start of application ====--");

    running = baka::Graphics::Init("Baka Engine", 1280, 720, validation);

    if(running)
    {
        baka::Input::Init();
        
        BakaMain(argc, argv);
    }

    frames.Start();
    bakalog("--==== Update of application ====--");
    while(running)
    {
        if(frames.GetTicks() >= 1.0f)
        {
            frames.Start();
            bakalog("fps %u", fps);
            fps = 0;
        }
        fps++;

        baka::Input::Update();

        if( baka::Input::QuitRequested() || baka::Input::KeyPressed(SDL_SCANCODE_ESCAPE) )
        {
            running = false;
        }

        BakaUpdate(0.0f);

        baka::Graphics::RenderBegin();
        baka::Graphics::RenderEnd();
    }

    bakalog("--==== End of application ====--");

    vkDeviceWaitIdle(baka::Graphics::GetDefaultLogicalDevice());

    return 0;
}