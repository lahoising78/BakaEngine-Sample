#include "baka_graphics.h"
#include "baka_logger.h"
#include "baka_vk_extensions.h"
#include "baka_vk_validation.h"
#include "baka_vk_queues.h"
#include "baka_vk_swapchain.h"
#include "baka_mesh.h"
#include "baka_texture.h"
#include "baka_vk_pipeline.h"
#include "baka_model.h"
#include <SDL2/SDL_vulkan.h>

namespace baka
{
    std::vector<const char *> Graphics::sdl_vulkan_extension_names;
    SDL_Window *Graphics::window = nullptr;
    VkApplicationInfo Graphics::vk_app_info;
    VkInstanceCreateInfo Graphics::vk_instance_info;
    VkInstance Graphics::vk_instance;
    bool Graphics::validation;
    VkDebugUtilsMessengerEXT Graphics::debug_callback;
    std::vector<VkPhysicalDevice> Graphics::devices;
    VkPhysicalDevice Graphics::gpu;
    VkSurfaceKHR Graphics::surface;
    VkPhysicalDeviceFeatures Graphics::device_features;
    VkDevice Graphics::device;
    bool Graphics::logical_device_created;
    VulkanPipeline *Graphics::pipe;
    VkSemaphore Graphics::render_complete;
    VkSemaphore Graphics::present_complete;
    uint32_t Graphics::current_cmd_bf;
    VulkanCommand Graphics::render_vk_command;

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

        queue_manager.SetupDeviceQueues(Graphics::device);
        baka_swap.Init(gpu, device, surface, width, height);

        baka_mesh.Init(1024);
        baka_texture_manager.Init(1024);

        baka_pipeline_manager.Init(4);
        pipe = baka_pipeline_manager.CreateBasicModel(
            device, 
            "baka_engine/shaders/always_vert.sprv", 
            "baka_engine/shaders/always_frag.sprv", 
            // "baka_engine/shaders/default_vert.sprv", 
            // "baka_engine/shaders/default_frag.sprv", 
            baka_swap.GetSwapchainExtent(), 
            1024
        );
        baka_model_manager.Init(1024, baka_swap.GetSwapchainLength(), device);

        render_vk_command = VulkanCommand(
            device, 
            baka_swap.GetImageCount(), 
            queue_manager.GetQueueFamily(VulkanQueueType::QUEUE_TYPE_GRAPHICS)
        );

        /* we need a depth image for depth test and stuff, but the swap chain doesn't create it automatically, so create it */
        baka_swap.CreateDepthImage();
        baka_swap.CreateFramebuffers(pipe);

        /** 
         * when we start rendering, we are going to need semaphores to tell the program when it is ok to continue 
         * https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Rendering_and_presentation#page_Semaphores */
        CreateSync();

        bakalog("baka graphics initialized");
        return true;
    }

    bool Graphics::Setup( const char *windowName, int width, int height)
    {
        uint32_t windowFlags = SDL_WINDOW_VULKAN;
        uint32_t sdlVulkanExtensionCount = 0;
        uint32_t count = 0;
        VkResult res;
        VkDeviceCreateInfo createInfo;

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
            baka_validation.Init();
            
            baka_validation.RemoveLayer("VK_LAYER_LUNARG_vktrace");
            baka_validation.RemoveLayer("VK_LAYER_LUNARG_device_simulation");
            baka_validation.RemoveLayer("VK_LAYER_LUNARG_api_dump");
            baka_validation.RemoveLayer("VK_LAYER_VALVE_steam_fossilize_64");
            baka_validation.RemoveLayer("VK_LAYER_VALVE_steam_fossilize_32");
            baka_validation.RemoveLayer("VK_LAYER_VALVE_steam_overlay_64");
            baka_validation.RemoveLayer("VK_LAYER_VALVE_steam_overlay_32");

            vk_instance_info.enabledLayerCount = baka_validation.GetLayerCount();
            vk_instance_info.ppEnabledLayerNames = baka_validation.GetLayerNames();
            instance_extensions.EnableExtension("VK_EXT_debug_utils");
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

        if(validation)
        {
            SetupDebug();
        }

        vkEnumeratePhysicalDevices(vk_instance, &count, nullptr);
        bakalog("Found %u %s for Vulkan", count, count > 1? "devices" : "device");
        if(!count)
        {
            bakaerr("Could not find a device for application");
            return false;
        }

        devices.resize(count);
        vkEnumeratePhysicalDevices(vk_instance, &count, devices.data());

        gpu = devices[0];

        SDL_Vulkan_CreateSurface(window, vk_instance, &surface);
        queue_manager.Init(gpu, surface);

        device_extensions.Init(gpu);
        device_extensions.EnableExtension("VK_KHR_swapchain");

        createInfo = GetDeviceCreateInfo();

        res = vkCreateDevice(gpu, &createInfo, nullptr, &device);
        if(res != VK_SUCCESS)
        {
            bakaerr("Unable to create logical device");
            return false;
        }
        logical_device_created = true;
        
        return true;
    }

    void Graphics::CreateSync()
    {
        bakalog("graphics create sync");

        CreateSingleSemaphore(&render_complete);
        CreateSingleSemaphore(&present_complete);
    }

    void Graphics::CreateSingleSemaphore(VkSemaphore *sem)
    {
        VkResult res;
        VkSemaphoreCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        res = vkCreateSemaphore(device, &info, nullptr, sem);
        if(res != VK_SUCCESS)
        {
            bakaerr("semaphore creation failed with error code %d", res);
        }
    }

    void Graphics::CreateSingleFence(VkFence *fence)
    {
        VkResult res;
        VkFenceCreateInfo info = {};

        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        
        res = vkCreateFence(device, &info, nullptr, fence);
        if(res != VK_SUCCESS)
        {
            bakaerr("fence creation failed with error code %d", res);
            return;
        }

        vkResetFences(device, 1, fence);
    }

    void Graphics::Close()
    {
        bakalog("closing baka graphics");

        if(render_complete != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(device, render_complete, nullptr);
        }

        if(render_complete != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(device, present_complete, nullptr);
        }

        render_vk_command.Free();

        if(logical_device_created)
        {
            vkDestroyDevice(device, nullptr);
        }

        CloseDebug();

        if(surface)
        {
            vkDestroySurfaceKHR(vk_instance, surface, nullptr);
        }
        if(window)
        {
            SDL_DestroyWindow( window );
        }
        
        if(vk_instance)
        {
            vkDestroyInstance(vk_instance, nullptr);
        }

    }

    VkDeviceCreateInfo Graphics::GetDeviceCreateInfo()
    {
        VkDeviceCreateInfo createInfo = {};
        uint32_t count;

        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queue_manager.GetCreateInfo(&count);
        createInfo.queueCreateInfoCount = count;
        
        device_features.samplerAnisotropy = VK_TRUE;
        
        createInfo.pEnabledFeatures = &device_features;

        createInfo.enabledExtensionCount = device_extensions.GetExtensionCount();
        createInfo.ppEnabledExtensionNames = device_extensions.GetExtensionNames();

        if(validation)
        {
            createInfo.enabledLayerCount = baka_validation.GetLayerCount();
            createInfo.ppEnabledLayerNames = baka_validation.GetLayerNames();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        return createInfo;
    }

    VkImageView Graphics::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
    {
        VkImageView imageView;
        VkImageViewCreateInfo viewInfo = {};

        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &viewInfo, NULL, &imageView) != VK_SUCCESS)
        {
            bakawarn("failed to create texture image view!");
            return VK_NULL_HANDLE;
        }

        return imageView;
    }

    VkDevice Graphics::GetDefaultLogicalDevice()
    {
        return device;
    }

    uint32_t Graphics::RenderBegin()
    {
        /** 
         * to start rendering, we need to get an image from the swapchain.
         * this is done by using vkAcquireNextImageKHR */

        vkAcquireNextImageKHR(
            device, 
            baka_swap.GetSwapchain(),
            /* this is a timeout in nanoseconds for an image to become avilable.
            setting it to UIT64_MAX turns off the timeout */
            UINT64_MAX,
            /* wait for presentation to be complete */
            present_complete,
            VK_NULL_HANDLE,
            /* index of image */
            &current_cmd_bf
        );

        render_vk_command.SetCurrentCmdBuffer(current_cmd_bf);
        render_vk_command.BeginCmdBuffer();

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pipe->GetRenderPass();
        renderPassInfo.framebuffer = baka_swap.GetFramebuffer(current_cmd_bf);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = baka_swap.GetSwapchainExtent();

        VkClearValue clearval = {0.0f, 0.0f, 1.0f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearval;

        vkCmdBindPipeline(render_vk_command.GetCurrentCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipe->pipeline);
        vkCmdBeginRenderPass(render_vk_command.GetCurrentCmdBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        return current_cmd_bf;
    }

    void Graphics::RenderEnd()
    {
        VkResult res;
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        vkCmdEndRenderPass(render_vk_command.GetCurrentCmdBuffer());
        vkEndCommandBuffer(render_vk_command.GetCurrentCmdBuffer());

        std::vector<VkSubmitInfo> submitInfoVec(1);
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &present_complete;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &render_vk_command.GetCurrentCmdBuffer();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &render_complete;
        submitInfoVec[0] = submitInfo;

        // vkResetFences(device, 1, &wait_fences[current_cmd_index]);

        // res = vkQueueSubmit(queue_manager.GetQueue(QUEUE_TYPE_GRAPHICS), 1, &submitInfo, VK_NULL_HANDLE);
        render_vk_command.SubmitQueue(queue_manager.GetQueue(QUEUE_TYPE_GRAPHICS), submitInfoVec, true);

        VkSwapchainKHR swaps[] = {baka_swap.GetSwapchain()};
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pWaitSemaphores = &render_complete;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pSwapchains = swaps;
        presentInfo.swapchainCount = 1;
        presentInfo.pImageIndices = &current_cmd_bf;

        res = vkQueuePresentKHR(queue_manager.GetQueue(QUEUE_TYPE_PRESENT), &presentInfo);
        if(res != VK_SUCCESS)
        {
            bakaerr("unable to present frame");
            return;
        }
    }
    
    /* VULKAN DEBUG */
    void Graphics::SetupDebug()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = Graphics::DebugCallback;
        createInfo.pUserData = nullptr;

        CreateDebugUtilsMessengerEXT(vk_instance, &createInfo, nullptr, &debug_callback);
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Graphics::DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    )
    {
        bakaerr("Vulkan Debug [%d]:%s", messageSeverity, pCallbackData->pMessage);
        return VK_FALSE;
    }

    VkResult Graphics::CreateDebugUtilsMessengerEXT(
        VkInstance instance, 
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, 
        VkDebugUtilsMessengerEXT* pDebugMessenger
    )
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void Graphics::CloseDebug()
    {
        if(validation)
        {
            DestroyDebugUtilsMessengerEXT(vk_instance, debug_callback, nullptr);
        }
    }

    void Graphics::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) 
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

}