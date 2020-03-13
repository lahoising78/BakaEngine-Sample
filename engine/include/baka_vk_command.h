#ifndef _BAKA_VK_COMMAND_H_
#define _BAKA_VK_COMMAND_H_

#include <vulkan/vulkan.h>
#include <vector>

namespace baka
{
    class VulkanCommand
    {
    public:
        VulkanCommand(VkDevice device, uint32_t bufferCount, int32_t queueFamIndex);
        VulkanCommand() {}
        void Free();

    private:
        VkCommandPool command_pool;
        std::vector<VkCommandBuffer> command_buffers;
        VkDevice device;
        friend class Graphics;
    };
}

#endif