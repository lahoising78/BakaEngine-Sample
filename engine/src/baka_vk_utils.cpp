#include "baka_logger.h"
#include "baka_vk_utils.h"
#include "baka_vk_queues.h"

namespace baka
{
    VkPhysicalDevice gpu;
    VkDevice device;

    VkCommandPool cmdPool;
    std::vector<VkCommandBuffer> cmdBuffers;
    VulkanQueueType cmdQueue;

    /* find stuff */
    void SetupUtils(VkPhysicalDevice physical, VkDevice logical)
    {
        gpu = physical;
        device = logical;

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queue_manager.GetQueueFamily(cmdQueue = QUEUE_TYPE_GRAPHICS);

        VkResult res = vkCreateCommandPool(device, &poolInfo, nullptr, &cmdPool);
        if(res != VK_SUCCESS)
        {
            bakaerr("command pool creation for utils failed with error code %d", res);
            return;
        }

        cmdBuffers.resize(1);
        VkCommandBufferAllocateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        bufferInfo.commandBufferCount = cmdBuffers.size();
        bufferInfo.commandPool = cmdPool;
        bufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        res = vkAllocateCommandBuffers(device, &bufferInfo, cmdBuffers.data() );
        if(res != VK_SUCCESS)
        {
            bakaerr("command buffer allocation for utils failed with error code %d", res);
            vkDestroyCommandPool(device, cmdPool, nullptr);
            return;
        }
    }

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
    
        /* query supported device memory properties */
        vkGetPhysicalDeviceMemoryProperties(gpu, &memProperties);

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

    VkCommandBuffer BeginSingleTimeCommand()
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VkResult res = vkBeginCommandBuffer(cmdBuffers[0], &beginInfo);
        if(res != VK_SUCCESS)
        {
            bakawarn("command buffer begin for single time failed with error code %d", res);
            return VK_NULL_HANDLE;
        }

        return cmdBuffers[0];
    }

    void EndSingleTimeCommand( VkCommandBuffer cmd )
    {
        if(cmd == VK_NULL_HANDLE) return;

        vkEndCommandBuffer(cmd);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;

        vkQueueSubmit(queue_manager.GetQueue(cmdQueue), 1, &submitInfo, VK_NULL_HANDLE );
        vkQueueWaitIdle(queue_manager.GetQueue(cmdQueue));

        vkResetCommandPool(device, cmdPool, 0);
    }


    /* buffer utils */
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkResult res;

        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        res = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
        if (res != VK_SUCCESS) {
            bakaerr("buffer creation failed with error code %d", res);
            return;
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        res = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
        if (res != VK_SUCCESS) {
            bakaerr("memory allocation failed with error code %d", res);
            return;
        }

        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }

    void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize bufferSize)
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommand();

        VkBufferCopy copyRegion = {};
        copyRegion.size = bufferSize;
        vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

        EndSingleTimeCommand(commandBuffer);
    }
}