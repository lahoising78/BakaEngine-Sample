#include "Baka.h"
#include "baka_logger.h"
#include "baka_input.h"
#include "baka_graphics.h"

int BakaApplication::Main(int argc, char *argv[])
{
    bool validations = false;
    for(int i = 1; i < argc; i++)
    {
        if( strcmp(argv[i], "-vv") == 0 || strcmp(argv[i], "--validations") == 0 )
        {
            validations = true;
        }
    }

    #ifdef VULKAN_AVAILABLE
    vk_graphics = new baka::VulkanGraphics("Baka Engine", validations);
    #endif

    #ifdef OPEN_GL_AVAILABLE
    gl_graphics = new baka::GLGraphics();
    #endif

    baka::Graphics::Init("Baka Engine", 1280, 720, this);

    return 0;
}

void BakaApplication::Update(float deltaTime)
{
    if( baka::Input::KeyPressed(SDL_SCANCODE_R) )
    {
        bakalog("you just pressed R");
    }
}

BakaApplication::~BakaApplication()
{
    if(vk_graphics)
        delete vk_graphics;

    if(gl_graphics)
        delete gl_graphics;
}