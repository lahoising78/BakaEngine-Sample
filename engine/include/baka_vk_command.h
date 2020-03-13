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
        VkResult BeginCmdBuffer();
        void SetCurrentCmdBuffer( uint32_t index ) { current_buf = index; }
        VkCommandBuffer &GetCurrentCmdBuffer() { return command_buffers[current_buf]; }
        VkResult SubmitQueue( VkQueue queue, std::vector<VkSubmitInfo> &submitInfo, bool wait_fence );
        void WaitFence();

    private:
        VkCommandPool command_pool;
        std::vector<VkCommandBuffer> command_buffers;
        std::vector<bool> buffer_submitted;
        std::vector<VkFence> fences;
        uint32_t current_buf;
        VkDevice device;
        friend class Graphics;
    };
}

#endif