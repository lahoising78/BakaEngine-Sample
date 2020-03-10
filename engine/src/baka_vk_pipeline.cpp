#include "baka_logger.h"
#include "baka_vk_swapchain.h"
#include "baka_vk_pipeline.h"

namespace baka
{
    VulkanPipelineManager baka_pipeline_manager = {};

    void VulkanPipelineManager::Init(uint32_t count)
    {
        bakalog("init pipeline manager");
        if(!count)
        {
            bakaerr("cannot init pipeline manager with 0 pipelines");
            return;
        }
        pipeline_list.resize(count);

        swap_length = baka_swap.GetSwapchainLength();
    }

    VulkanPipeline *VulkanPipelineManager::CreateBasicModel(VkDevice device, const char *vert, const char *frag, VkExtent2D extent, uint32_t count)
    {
        return nullptr;
    }
}