#include "baka_logger.h"
#include "baka_vk_queues.h"
#include <cstdlib>

namespace Baka
{
    static VulkanQueue graphics_queue;
    static VulkanQueue present_queue;
    static VulkanQueue transfer_queue;

    void VulkanQueueManager::Init(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        uint32_t i;
        VkBool32 supported;

        graphics_queue.family = -1;
        transfer_queue.family = -1;
        present_queue.family = -1;

        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
        bakalog("Discovered %u queue families", queue_family_count);
        if(!queue_family_count)
        {
            return;
        }

        queue_properties.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_properties.data());

        for(i = 0; i < queue_family_count; i++)
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supported);

            if( queue_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT )
            {
                graphics_queue.family = i;
                graphics_queue.priority = 1.0f;
                bakalog("queue %u handles graphics", i);
            }

            if( queue_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT )
            {
                transfer_queue.family = i;
                transfer_queue.priority = 1.0f;
                bakalog("queue %u handles transfer", i);
            }

            if( supported )
            {
                present_queue.family = i;
                present_queue.priority = 1.0f;
                bakalog("queue %u handles presentation", i);
            }
        }

        if(graphics_queue.family != -1)
        {
            create_info.push_back(graphics_queue.GetQueueInfo());
        }
        if(present_queue.family != -1 && present_queue.family != graphics_queue.family)
        {
            create_info.push_back(present_queue.GetQueueInfo());
        }
    }

    void VulkanQueueManager::Close()
    {
        bakalog("closing vulkan queue manager");
    }

    const VkDeviceQueueCreateInfo *VulkanQueueManager::GetCreateInfo(uint32_t *count)
    {
        if(count) *count = create_info.size();
        return create_info.data();
    }

    void VulkanQueueManager::SetupDeviceQueues(VkDevice device)
    {
        if (graphics_queue.family != -1)
        {
            vkGetDeviceQueue(device, graphics_queue.family, 0, &graphics_queue.queue);
        }
        if (present_queue.family != -1)
        {
            vkGetDeviceQueue(device, present_queue.family, 0, &present_queue.queue);
        }
        if (transfer_queue.family != -1)
        {
            vkGetDeviceQueue(device, transfer_queue.family, 0, &transfer_queue.queue);
        }
    }

    int32_t VulkanQueueManager::GetQueueFamily( VulkanQueueType pType )
    {
        switch (pType)
        {
        case QUEUE_TYPE_GRAPHICS: return graphics_queue.family; break;
        case QUEUE_TYPE_PRESENT: return present_queue.family; break;
        case QUEUE_TYPE_TRANSFER: return transfer_queue.family; break;
        default: return -1;
        }
    }

    VkDeviceQueueCreateInfo VulkanQueue::GetQueueInfo()
    {
        VkDeviceQueueCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createInfo.queueFamilyIndex = family;
        createInfo.queueCount = 1;
        createInfo.pQueuePriorities = &priority;
        return createInfo;
    }
}