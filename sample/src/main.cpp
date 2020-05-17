#include "Baka.h"
#include "baka_logger.h"
#include "baka_input.h"
#include "baka_graphics.h"

int BakaApplication::Main(int argc, char *argv[])
{
    bool validations = true;
    for(int i = 1; i < argc; i++)
    {
        if( strcmp(argv[i], "-nv") == 0 || strcmp(argv[i], "--no-validations") == 0 )
        {
            validations = false;
        }
    }

    vk_graphics = new baka::VulkanGraphics("Baka Engine", validations);

    uint32_t flags = 0;
    if(vk_graphics) flags |= baka::GraphicAPI::VULKAN;

    baka::Graphics::Init("Baka Engine", 1280, 720, flags);

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
}