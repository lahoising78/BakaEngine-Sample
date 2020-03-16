#include "Baka.h"
#include "baka_logger.h"
#include "baka_input.h"
#include "baka_vectors.h"
#include "baka_matrix.h"
#include "baka_mesh.h"
#include <math.h>
#include <iostream>

int BakaMain(int argc, char *argv[])
{
    std::vector<baka::Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };
    std::vector<uint32_t> faces = {
        0, 1, 2, 2, 3, 0
    };

    baka::Mesh *mesh = baka::baka_mesh.Load( vertices , faces, "cube");

    return 0;
}

void BakaUpdate(float deltaTime)
{
    if( baka::Input::KeyPressed(SDL_SCANCODE_R) )
    {
        bakalog("you just pressed R");
    }
}