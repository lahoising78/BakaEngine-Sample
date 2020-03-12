#include "baka_logger.h"
#include "baka_vk_swapchain.h"
#include "baka_vk_queues.h"
#include <algorithm>
#include "baka_graphics.h"

namespace baka
{
    static void SwapchainClose();
    VulkanSwapchain baka_swap;

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

        count = 0;
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

        Create(device, surface);
        this->device = device;

        atexit(SwapchainClose);
    }

    void SwapchainClose()
    {
        baka_swap.Close();
    }

    void VulkanSwapchain::Close()
    {
        uint32_t i;
        bakawarn("closing swap chain");

        if(depth_image_view != VK_NULL_HANDLE)
        {
            vkDestroyImageView(device, depth_image_view, nullptr);
        }

        if(depth_image != VK_NULL_HANDLE)
        {
            vkDestroyImage(device, depth_image, nullptr);
        }

        if(depth_image_memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, depth_image_memory, nullptr);
        }

        if(swapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(device, swapchain, nullptr);
        }

        for(i = 0; i < image_views.size(); i++)
        {
            vkDestroyImageView(device, image_views[i], nullptr);
        }

    }

    void VulkanSwapchain::CreateDepthImage()
    {
        CreateImage(
            extent.width, extent.height,
            Graphics::GetDefaultPipeline()->FindDepthFormat(),
            /** 
             * tiling specifies tiling arrangement in memory. 
             * as far as im aware, we would only use the other type
             * of tiling if we were to copy data into the image 
             * https://www.reddit.com/r/vulkan/comments/71k4gy/why_is_vk_image_tiling_linear_so_limited/ */
            VK_IMAGE_TILING_OPTIMAL,                                
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            /* make it as optimal as possible for the device */
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &depth_image, &depth_image_memory
        );

        /* get image view for depth image */
        depth_image_view = Graphics::CreateImageView(
            depth_image, 
            Graphics::GetDefaultPipeline()->FindDepthFormat(), 
            VK_IMAGE_ASPECT_DEPTH_BIT
        );

        /** 
         * eventually we will need to transition the image to from undefined layout to depth stencil attachment.
         * we told the render pass to do that already, so yeah. but you could do it here */
    }

    bool VulkanSwapchain::CreateImage(uint32_t w, uint32_t h, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory)
    {
        VkImageCreateInfo imageInfo = {};
        VkMemoryAllocateInfo allocInfo = {};
        VkMemoryRequirements memRequirements;
        VkResult res;

        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = w;
        imageInfo.extent.height = h;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &imageInfo, NULL, image) != VK_SUCCESS)
        {
            bakawarn("failed to create image!");
            return false;
        }

        vkGetImageMemoryRequirements(device, *image, &memRequirements);

        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        /* allocate device memory */
        res = vkAllocateMemory(device, &allocInfo, NULL, imageMemory);
        if (res != VK_SUCCESS)
        {
            bakawarn("image memory allocation failed with error code %d", res);
            vkDestroyImage(device, *image, nullptr);
            return false;
        }

        /* bind memory to image */
        vkBindImageMemory(device, *image, *imageMemory, 0);

        bakalog("depth image creation completed");
        return true;
    }

    uint32_t VulkanSwapchain::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
    
        /* query supported device memory properties */
        vkGetPhysicalDeviceMemoryProperties(Graphics::GetDefaultPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            /* matches filter and property flags */
            if ((typeFilter & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties))
            {
                return i;
            }
        }

        bakawarn("failed to find suitable memory type");
        return 0;
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

        bakalog("there are %u images available", count);
        images.resize(count);
        vkGetSwapchainImagesKHR(device, swapchain, &count, images.data());
        bakalog("created swapchain with %u images", count);

        image_views.resize(count);
        for(count = 0; count < image_views.size(); count++)
        {
            image_views[count] = Graphics::CreateImageView( images[count], formats[chosen_format].format, VK_IMAGE_ASPECT_COLOR_BIT );
        }
    }
}