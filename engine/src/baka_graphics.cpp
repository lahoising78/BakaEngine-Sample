#include "baka_graphics.h"
#include "baka_logger.h"
#include "baka_vk_extensions.h"
#include <SDL2/SDL_vulkan.h>

namespace Baka
{
    std::vector<const char *> Graphics::sdl_vulkan_extension_names;
    SDL_Window *Graphics::window = nullptr;
    VkApplicationInfo Graphics::vk_app_info;
    VkInstanceCreateInfo Graphics::vk_instance_info;
    VkInstance Graphics::vk_instance;
    bool Graphics::validation;

    bool Graphics::Init( const char *windowName, int width, int height, bool validation )
    {
        if(SDL_Init( SDL_INIT_EVERYTHING ) != 0)
        {
            bakaerr("Failed to initialize SDL");
            return false;
        }
        atexit(SDL_Quit);

        Graphics::validation = validation;
        if(!Graphics::Setup(windowName, width, height))
        {
            return false;
        }

        atexit(Graphics::Close);

        bakalog("baka graphics initialized");
        return true;
    }

    bool Graphics::Setup( const char *windowName, int width, int height)
    {
        uint32_t windowFlags = SDL_WINDOW_VULKAN;
        uint32_t sdlVulkanExtensionCount = 0;
        VkResult res;

        window = SDL_CreateWindow(
            windowName,
            0, 0,
            width, height,
            windowFlags
        );

        if(!window)
        {
            bakaerr("Unable to create window");
            return false;
        }

        instance_extensions.Init();

        SDL_Vulkan_GetInstanceExtensions(window, &sdlVulkanExtensionCount, nullptr);
        if( sdlVulkanExtensionCount > 0 )
        {
            bakalog("SDL Vulkan Extensions count %u", sdlVulkanExtensionCount);
            sdl_vulkan_extension_names.resize(sdlVulkanExtensionCount);
            SDL_Vulkan_GetInstanceExtensions(window, &sdlVulkanExtensionCount, sdl_vulkan_extension_names.data());
            for(const char *name : sdl_vulkan_extension_names)
            {
                instance_extensions.EnableExtension( name );
            }
        }
        else
        {
            bakaerr("SDL window does not support Vulkan");
            return false;
        }

        vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vk_app_info.pNext = nullptr;
        vk_app_info.pApplicationName = windowName;
        vk_app_info.applicationVersion = 0;
        vk_app_info.pEngineName = "Baka Engine";
        vk_app_info.engineVersion = 0;
        vk_app_info.apiVersion = VK_API_VERSION_1_1;

        vk_instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        vk_instance_info.pNext = nullptr;
        vk_instance_info.pApplicationInfo = &vk_app_info;

        if(validation)
        {

        }
        else
        {
            vk_instance_info.enabledLayerCount = 0;
            vk_instance_info.ppEnabledLayerNames = nullptr;
        }

        vk_instance_info.ppEnabledExtensionNames = instance_extensions.GetExtensionNames();
        vk_instance_info.enabledExtensionCount = instance_extensions.GetExtensionCount();

        res = vkCreateInstance(&vk_instance_info, nullptr, &vk_instance);
        if( res != VK_SUCCESS )
        {
            bakaerr("Unable to create instance. Error %d", res);
            return false;
        }
        else
        {
            bakalog("VkInstance created successfully");
        }

        return true;
    }

    void Graphics::Close()
    {
        bakalog("closing baka graphics");
        if(window)
        {
            SDL_DestroyWindow( window );
        }

        vkDestroyInstance(vk_instance, nullptr);
    }
}