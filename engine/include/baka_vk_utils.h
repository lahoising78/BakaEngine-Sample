#ifndef _BAKA_UTILS_H_
#define _BAKA_UTILS_H_

#include <vulkan/vulkan.h>

namespace baka
{
    /* find stuff */
    void SetupUtils(VkPhysicalDevice, VkDevice);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    
    /* command utils */
    VkCommandBuffer BeginSingleTimeCommand();
    void EndSingleTimeCommand( VkCommandBuffer cmd );

    /* buffer utils */
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize bufferSize);
    // void SetupBuffer(VkBuffer &dstBuf, VkDeviceMemory &dstMem, )
}

#endif