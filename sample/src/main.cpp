#include "Baka.h"
#include "baka_logger.h"
#include "baka_input.h"
#include "baka_graphics.h"

int BakaApplication::Main(int argc, char *argv[])
{
    baka::Graphics::Init("Baka Engine", 1280, 720, 0);

    #ifdef VULKAN_AVAILABLE
    bakalog("vulkan available");
    vk_graphics = new baka::VulkanGraphics();
    #else
    bakaerr("vulkan not found");
    #endif

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

}