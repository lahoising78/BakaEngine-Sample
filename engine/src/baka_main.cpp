#include "Baka.h"
#include "baka_graphics.h"
#include "baka_input.h"
#include "SDL.h"
#include "baka_logger.h"

int main(int argc, char *argv[])
{
    bool running = false;
    bool validation = true;
    uint32_t frame = 0;
    bakalog("--==== Start of application ====--");

    running = baka::Graphics::Init("Baka Engine", 1280, 720, validation);

    if(running)
    {
        baka::Input::Init();
        
        BakaMain(argc, argv);
    }

    bakalog("--==== Update of application ====--");
    while(running)
    {
        baka::Input::Update();

        if( baka::Input::QuitRequested() || baka::Input::KeyPressed(SDL_SCANCODE_ESCAPE) )
        {
            running = false;
        }

        BakaUpdate(0.0f);

        bakalog("before begin");
        frame = baka::Graphics::RenderBegin();
        bakalog("middle");
        baka::Graphics::RenderEnd(frame);
        bakalog("after begin");
    }

    bakalog("--==== End of application ====--");

    vkDeviceWaitIdle(baka::Graphics::GetDefaultLogicalDevice());

    return 0;
}