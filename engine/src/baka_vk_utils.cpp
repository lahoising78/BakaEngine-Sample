#include "baka_logger.h"
#include "baka_vk_utils.h"

namespace baka
{
    VkPhysicalDevice gpu;
    VkDevice device;

    void SetupUtils(VkPhysicalDevice physical, VkDevice logical)
    {
        gpu = physical;
        device = logical;
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
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion = {};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }
}