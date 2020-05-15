#include "Baka.h"
#include "baka_logger.h"
#include "baka_input.h"
#include "baka_graphics.h"
#include <math.h>
#include <iostream>

baka::Input *baka_in;

int BakaMain(int argc, char *argv[])
{
    baka::Graphics::Init("Baka Engine", 1280, 720, 0);
    return 0;
}

void BakaUpdate(float deltaTime)
{
    if( baka::Input::KeyPressed(SDL_SCANCODE_R) )
    {
        bakalog("you just pressed R");
    }
}