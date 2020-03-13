#include "baka_logger.h"
#include "baka_vk_command.h"

namespace baka
{
    /* Multi threaded commands: https://stackoverflow.com/questions/38318818/multi-thread-rendering-vs-command-pools */

    /** 
     * to draw in vulkan you record the draw calls and operations using command buffers. 
     * this allows us to do all the hard work in advance and/or in multiple threads
     * https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Command_buffers
     * */
    VulkanCommand::VulkanCommand(VkDevice device, uint32_t bufferCount, int32_t queueFamIndex)
    {
        VkResult res;
        VkCommandPoolCreateInfo poolInfo = {};
        VkCommandBufferAllocateInfo bufferAllocInfo = {};

        this->device = device;

        /* a command buffer cannot be created by itself. it must be created from a command pool */
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamIndex;
        /* for now */
        poolInfo.flags = 0;

        res = vkCreateCommandPool(device, &poolInfo, nullptr, &command_pool);
        if(res != VK_SUCCESS)
        {
            bakaerr("command pool creation failed with error code %d", res);
            return;
        }

        command_buffers.resize(bufferCount);
        bufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        /** 
         * primary lets us submit commands for executions, but cannot be called from another command buffer
         * secondary is the complete opposite. this allows us to do reuse common operations from other command buffers
         * */
        bufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        bufferAllocInfo.commandPool = command_pool;
        bufferAllocInfo.commandBufferCount = command_buffers.size();

        res = vkAllocateCommandBuffers(device, &bufferAllocInfo, (VkCommandBuffer*)command_buffers.data());
        if(res != VK_SUCCESS)
        {
            bakaerr("command buffer allocation failed with error code %d", res);
            Free();
        }

        bakalog("created command pool and buffers");
    }

    void VulkanCommand::Free()
    {
        if(command_buffers.size() > 0 && command_buffers[0] != VK_NULL_HANDLE)
            vkFreeCommandBuffers(device, command_pool, command_buffers.size(), command_buffers.data());
        
        if(command_pool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(device, command_pool, nullptr);
        }

    }
}