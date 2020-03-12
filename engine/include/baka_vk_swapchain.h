#ifndef _BAKA_VK_SWAPCHAIN_H_
#define _BAKA_VK_SWAPCHAIN_H_

#include <vulkan/vulkan.h>
#include <vector>

namespace baka
{
    class VulkanSwapchain
    {
    public:
        void Init(VkPhysicalDevice gpu, VkDevice device, VkSurfaceKHR surface, uint32_t width, uint32_t height);
        void Close();
        VkSwapchainKHR GetSwapchain() {return swapchain;}
        uint32_t GetSwapchainLength() {return swapchain_count;}
        VkExtent2D GetSwapchainExtent() {return extent;}
        VkSurfaceFormatKHR GetChosenFormat() { return formats[chosen_format]; }

    private:
        uint32_t ChooseFormat();
        uint32_t ChoosePresentMode();
        VkExtent2D ConfigureExtent(uint32_t width, uint32_t height);
        void Create(VkDevice device, VkSurfaceKHR surface);

    private:
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        uint32_t chosen_format;
        std::vector<VkPresentModeKHR> present_modes;        /* how are we going to handle incoming images from swapchain */
        uint32_t chosen_present_mode;
        VkExtent2D extent;
        uint32_t swapchain_count;
        VkSwapchainKHR swapchain;
        std::vector<VkImage> images;
        std::vector<VkImageView> image_views;
        VkDevice device;
    };

    extern VulkanSwapchain baka_swap;
}

#endif