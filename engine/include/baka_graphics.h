#ifndef _BAKA_GRAPHICS_H_
#define _BAKA_GRAPHICS_H_

#include <SDL2/SDL.h>
#include <vector>
#include <vulkan/vulkan.h>

namespace Baka{
    class Graphics
    {
    public:
        static bool Init( const char *windowName, int width, int height, bool validation );

    private:
        static bool Setup( const char *windowName, int width, int height );
        static void Close();

    private:
        static SDL_Window *window;
        static std::vector<const char *> sdl_vulkan_extension_names;
        static VkApplicationInfo vk_app_info;
        static VkInstanceCreateInfo vk_instance_info;
        static VkInstance vk_instance;
        static bool validation;
    };
}

#endif