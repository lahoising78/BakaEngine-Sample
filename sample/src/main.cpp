#include "Baka.h"
#include "baka_logger.h"
#include "baka_input.h"
#include "baka_vectors.h"
#include <iostream>

int BakaMain(int argc, char *argv[])
{
    Vec3 a{0.0f, 12.0f, 142.0f};
    Vec3 b{12.0f, 636.0f, 200.0f};

    bakalog("hey from sample main");

    a += b;
    bakalog("a is %.2f %.2f %.2f", a.x, a.y, a.z);
    a -= {1.0f, 623.0f, 75.0f};
    bakalog("a is %.2f %.2f %.2f", a.x, a.y, a.z);

    return 0;
}

void BakaUpdate(float deltaTime)
{
    if( Baka::Input::KeyPressed(SDL_SCANCODE_R) )
    {
        bakalog("you just pressed R");
    }
}