#ifndef _BAKA_VK_PIPELINE_H_
#define _BAKA_VK_PIPELINE_H_

#include <vulkan/vulkan.h>
#include <vector>

namespace baka
{
    class VulkanPipeline
    {

    };

    class VulkanPipelineManager
    {
    public:
        void Init(uint32_t count);
        VulkanPipeline *CreateBasicModel(VkDevice device, const char *vert, const char *frag, VkExtent2D extent, uint32_t count);

    private:
        std::vector<VulkanPipeline> pipeline_list;
        uint32_t swap_length;
    };

    extern VulkanPipelineManager baka_pipeline_manager;
}

#endif