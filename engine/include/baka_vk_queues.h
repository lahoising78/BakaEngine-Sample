#ifndef _BAKA_VK_QUEUES_H_
#define _BAKA_VK_QUEUES_H_

#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>

namespace Baka
{
    enum VulkanQueueType
    {
        QUEUE_TYPE_NONE = 0,
        QUEUE_TYPE_GRAPHICS = 1,
        QUEUE_TYPE_PRESENT = 2,
        QUEUE_TYPE_TRANSFER = 3
    };

    class VulkanQueue
    {
    protected:
        VkDeviceQueueCreateInfo GetQueueInfo();

        int32_t family;
        float priority;
        VkQueue queue;

        friend class VulkanQueueManager;
    };

    class VulkanQueueManager
    {
    public:
        void Init(VkPhysicalDevice device, VkSurfaceKHR surface);
        void Close();
        const VkDeviceQueueCreateInfo *GetCreateInfo(uint32_t *count);
        void SetupDeviceQueues(VkDevice device);

        int32_t GetQueueFamily( VulkanQueueType qType );

    private:
        uint32_t queue_family_count;
        std::vector<VkQueueFamilyProperties> queue_properties;
        std::vector<VkDeviceQueueCreateInfo> create_info;
    };

    extern VulkanQueueManager queue_manager;
}

#endif