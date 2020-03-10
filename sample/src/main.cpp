#include "Baka.h"
#include "baka_logger.h"
#include "baka_input.h"
#include "baka_vectors.h"
#include "baka_matrix.h"
#include <math.h>
#include <iostream>

int BakaMain(int argc, char *argv[])
{
    return 0;
}

void BakaUpdate(float deltaTime)
{
    if( baka::Input::KeyPressed(SDL_SCANCODE_R) )
    {
        bakalog("you just pressed R");
    }
}