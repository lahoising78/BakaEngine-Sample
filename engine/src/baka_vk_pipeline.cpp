#include "baka_logger.h"
#include "baka_vk_swapchain.h"
#include "baka_shader.h"
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

    void VulkanPipelineManager::Close()
    {
        for(auto &pipe : pipeline_list)
        {
            pipe.Free();
        }
    }

    MANAGER_NEW(VulkanPipeline, VulkanPipelineManager, pipeline_list)

    VulkanPipeline *VulkanPipelineManager::CreateBasicModel(VkDevice device, const char *vert, const char *frag, VkExtent2D extent, uint32_t count)
    {
        VulkanPipeline *pipe = VulkanPipelineNew();
        if(!pipe) return nullptr;

        pipe->vertShaderCode = LoadData(vert);
        pipe->vertModule = CreateModule(pipe->vertShaderCode, device);

        pipe->fragShaderCode = LoadData(frag);
        pipe->fragModule = CreateModule(pipe->fragShaderCode, device);

        return pipe;
    }

    void VulkanPipeline::Free()
    {
        vkDestroyShaderModule(baka_pipeline_manager.GetDevice(), vertModule, nullptr);
        vkDestroyShaderModule(baka_pipeline_manager.GetDevice(), fragModule, nullptr);
    }
}