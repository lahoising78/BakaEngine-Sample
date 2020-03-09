#ifndef _BAKA_VK_QUEUES_H_
#define _BAKA_VK_QUEUES_H_

#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>

namespace Baka
{
    class VulkanQueue
    {
    protected:
        VkDeviceQueueCreateInfo GetQueueInfo();

        int32_t family;
        float priority;

        friend class VulkanQueueManager;
    };

    class VulkanQueueManager
    {
    public:
        void Init(VkPhysicalDevice device, VkSurfaceKHR surface);
        void Close();
        const VkDeviceQueueCreateInfo *GetCreateInfo(uint32_t *count);

    private:
        uint32_t queue_family_count;
        std::vector<VkQueueFamilyProperties> queue_properties;
        std::vector<VkDeviceQueueCreateInfo> create_info;
    };

    static VulkanQueueManager queue_manager;
}

#endif