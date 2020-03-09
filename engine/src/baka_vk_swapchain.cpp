#include "baka_logger.h"
#include "baka_vk_swapchain.h"
#include "baka_vk_queues.h"
#include <algorithm>
#include "baka_graphics.h"

namespace Baka
{
    static void SwapchainClose();
    static VkDevice default_device;
    static VkSwapchainKHR default_swapchain;
    static std::vector<VkImageView> *default_image_views;
    static std::vector<VkImage> *default_images;

    void VulkanSwapchain::Init(VkPhysicalDevice gpu, VkDevice device, VkSurfaceKHR surface, uint32_t width, uint32_t height)
    {
        uint32_t i;
        uint32_t count;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &capabilities);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &count, nullptr);

        bakalog("device supports %u surface formats", count);
        if( count != 0 )
        {
            formats.resize(count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &count, formats.data());
            for(i = 0; i < formats.size(); i++)
            {
                bakalog("surface format #%u, format %d, color space %d", i, formats[i].format, formats[i].colorSpace);
            }
        }

        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &count, nullptr);
        bakalog("%u present modes available", count);
        if( count != 0 )
        {
            present_modes.resize(count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &count, present_modes.data());
            for(i = 0; i < present_modes.size(); i++)
            {
                bakalog("present mode %u is %u", i, present_modes[i]);
            }
        }

        chosen_format = ChooseFormat();
        bakalog("the chosen format was %u", chosen_format);

        chosen_present_mode = ChoosePresentMode();
        bakalog("the chose presentation mode was %u", chosen_present_mode);

        extent = ConfigureExtent(width, height);
        bakalog("final extent: %u %u", extent.width, extent.height);

        this->Create(device, surface);
        this->device = device;

        default_device = device;
        default_swapchain = swapchain;
        default_image_views = &image_views;
        default_images = &images;

        atexit(SwapchainClose);
    }

    void SwapchainClose()
    {
        uint32_t i;
        vkDestroySwapchainKHR(default_device, default_swapchain, nullptr);

        for(i = 0; i < default_image_views->size(); i++)
        {
            vkDestroyImageView(default_device, (*default_image_views)[i], nullptr);
        }

        for(i = 0; i < default_images->size(); i++)
        {
            vkDestroyImage(default_device, (*default_images)[i], nullptr);
        }
    }

    uint32_t VulkanSwapchain::ChooseFormat()
    {
        uint32_t i;
        int chosen = -1;
        for (i = 0; i < formats.size(); i++)
        {
            if ((formats[i].format == VK_FORMAT_B8G8R8A8_UNORM) &&
                (formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
                return i;
            chosen = (int)i;
        }
        return chosen;
    }

    uint32_t VulkanSwapchain::ChoosePresentMode()
    {
        uint32_t i;
        int chosen = -1;
        for (i = 0; i < formats.size(); i++)
        {
            if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
                return i;
            chosen = (int)i;
        }
        return chosen;
    }

    VkExtent2D VulkanSwapchain::ConfigureExtent(uint32_t width, uint32_t height)
    {
        VkExtent2D actualExtent;
        bakalog("Requested resolution: (%u,%u)",width,height);
        bakalog("Minimum resolution: (%u,%u)", capabilities.minImageExtent.width, capabilities.minImageExtent.height);
        bakalog("Maximum resolution: (%u,%u)", capabilities.maxImageExtent.width, capabilities.maxImageExtent.height);
        
        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(width, capabilities.maxImageExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(height, capabilities.maxImageExtent.height));
        return actualExtent;
    }

    void VulkanSwapchain::Create(VkDevice device, VkSurfaceKHR surface)
    {
        int32_t graphicsFamily;
        int32_t presentFamily;
        int32_t transferFamily;
        uint32_t queueFamilyIndices[3];
        VkSwapchainCreateInfoKHR createInfo = {};
        uint32_t count;
        VkResult res;

        bakalog("minimum images needed for swapchain: %u", capabilities.minImageCount);
        bakalog("max images for swapchain: %u", capabilities.maxImageCount);

        swapchain_count = capabilities.minImageCount + 1;
        if(capabilities.maxImageCount) swapchain_count = std::min(swapchain_count, capabilities.maxImageCount);
        bakalog("using %u images for swapchain", swapchain_count);

        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = swapchain_count;
        createInfo.imageFormat = formats[chosen_format].format;
        createInfo.imageColorSpace = formats[chosen_format].colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;                                    /* layers for each swapchain. maybe useful for ui overlay? */
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;        /* what are we doing with the images? we are going to present it directly to the screen */

        graphicsFamily = queue_manager.GetQueueFamily(QUEUE_TYPE_GRAPHICS);
        presentFamily = queue_manager.GetQueueFamily(QUEUE_TYPE_PRESENT);
        transferFamily = queue_manager.GetQueueFamily(QUEUE_TYPE_TRANSFER);
        queueFamilyIndices[0] = graphicsFamily;
        queueFamilyIndices[1] = presentFamily;
        queueFamilyIndices[2] = transferFamily;

        if( graphicsFamily != presentFamily )
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 3;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = capabilities.currentTransform;        /* do something with the image before presenting, like flipping and rotation */
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;  /* this is for the window. doing stuff with alpha channel if we want it to blend with other windows */

        createInfo.presentMode = present_modes[chosen_present_mode];
        createInfo.clipped = VK_TRUE;                                   /* if true, don't render the pixels that you cannot see, like a window is in front of them */

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        res = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain);
        if( res != VK_SUCCESS )
        {
            bakaerr("swapchain creation failed with error code %u", res);
            return;
        }
        bakalog("swapchain has been created with length %u", swapchain_count);

        vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr);
        if(count == 0)
        {
            bakalog("unable to create images for swapchain");
            return;
        }

        images.resize(count);
        vkGetSwapchainImagesKHR(device, swapchain, &count, images.data());
        bakalog("created swapchain with %u images", count);

        image_views.resize(count);
        for(count = 0; count < image_views.size(); count++)
        {
            image_views[count] = Graphics::CreateImageView( images[count], formats[chosen_format].format );
        }
    }

    VkDevice VulkanSwapchain::GetDevice()
    {
        return device;
    }

    VkSwapchainKHR VulkanSwapchain::GetSwapchain()
    {
        return swapchain;
    }

}