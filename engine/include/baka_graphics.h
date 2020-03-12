#ifndef _BAKA_GRAPHICS_H_
#define _BAKA_GRAPHICS_H_

#include <SDL2/SDL.h>
#include <vector>
#include <vulkan/vulkan.h>
#include "baka_vk_pipeline.h"
#include "baka_vk_command.h"

namespace baka{
    class Graphics
    {
    public:
        static bool Init( const char *windowName, int width, int height, bool validation );
        static VkImageView CreateImageView(VkImage image, VkFormat format);
        static VkDevice GetDefaultLogicalDevice();
        static VkPhysicalDevice GetDefaultPhysicalDevice() { return gpu; }
        static VulkanPipeline *GetDefaultPipeline() { return pipe; }

    private:
        static bool Setup( const char *windowName, int width, int height );
        static void Close();
        static void SetupDebug();
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);
        static VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance, 
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
            const VkAllocationCallbacks* pAllocator, 
            VkDebugUtilsMessengerEXT* pDebugMessenger);
        static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
        static void CloseDebug();
        static VkDeviceCreateInfo GetDeviceCreateInfo();

    private:
        static SDL_Window *window;
        static std::vector<const char *> sdl_vulkan_extension_names;
        static VkApplicationInfo vk_app_info;
        static VkInstanceCreateInfo vk_instance_info;
        static VkInstance vk_instance;
        static bool validation;
        static VkDebugUtilsMessengerEXT debug_callback;
        static std::vector<VkPhysicalDevice> devices;
        static VkPhysicalDevice gpu;
        static VkSurfaceKHR surface;
        static VkPhysicalDeviceFeatures device_features;
        static VkDevice device;
        static bool logical_device_created;
        static VulkanPipeline *pipe;
        static VulkanCommand render_vk_command;
    };
}

#endif