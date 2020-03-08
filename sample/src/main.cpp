#include "Baka.h"
#include "baka_logger.h"
#include "baka_input.h"
#include <iostream>

int BakaMain(int argc, char *argv[])
{
    bakalog("hey from sample main");

    return 0;
}

void BakaUpdate(float deltaTime)
{
    if( Baka::Input::KeyPressed(SDL_SCANCODE_R) )
    {
        bakalog("you just pressed R");
    }
}