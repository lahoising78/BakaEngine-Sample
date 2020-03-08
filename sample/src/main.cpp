#include "Baka.h"
#include "baka_logger.h"
#include "baka_input.h"
#include "baka_vectors.h"
#include "baka_matrix.h"
#include <math.h>
#include <iostream>

int BakaMain(int argc, char *argv[])
{
    Vec3 a{0.0f, 12.0f, 142.0f};
    Vec3 b{12.0f, 636.0f, 200.0f};
    Matrix4 mat;

    bakalog("hey from sample main");

    a += b;
    bakalog("a is %.2f %.2f %.2f", a.x, a.y, a.z);
    a -= {1.0f, 623.0f, 75.0f};
    bakalog("a is %.2f %.2f %.2f", a.x, a.y, a.z);

    matrix_zero(mat);
    matrix_print(mat);
    matrix_identity(mat);
    matrix_print(mat);
    matrix_view(mat, {2, 40, 2}, {0, 0, 0}, {0, 0, 1});
    matrix_print(mat);
    matrix_identity(mat);
    matrix_perspective(mat, 45 * M_PI / 180.0f, 1280/720.0f, 0.1f, 100);
    matrix_print(mat);

    return 0;
}

void BakaUpdate(float deltaTime)
{
    if( Baka::Input::KeyPressed(SDL_SCANCODE_R) )
    {
        bakalog("you just pressed R");
    }
}