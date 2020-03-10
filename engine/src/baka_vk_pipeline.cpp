#include "baka_logger.h"
#include "baka_vk_swapchain.h"
#include "baka_shader.h"
#include "baka_vk_pipeline.h"
#include <cstdlib>

namespace baka
{
    VulkanPipelineManager baka_pipeline_manager = {};
    static void BakaPipelineManagerClose()
    {
        baka_pipeline_manager.Close();
    }

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
        std::atexit(BakaPipelineManagerClose);
    }

    void VulkanPipelineManager::Close()
    {
        bakawarn("closing vulkan pipeline manager");
        for(auto &pipe : pipeline_list)
        {
            pipe.Free();
        }
    }

    /* Vulkan Pipeline New */
    MANAGER_NEW(VulkanPipeline, VulkanPipelineManager, pipeline_list)

    VulkanPipeline *VulkanPipelineManager::CreateBasicModel(VkDevice device, const char *vert, const char *frag, VkExtent2D extent, uint32_t count)
    {
        VulkanPipeline *pipe = VulkanPipelineNew();
        if(!pipe) return nullptr;

        pipe->vertShaderCode = LoadData(vert);
        pipe->vertModule = CreateModule(pipe->vertShaderCode, device);

        pipe->fragShaderCode = LoadData(frag);
        pipe->fragModule = CreateModule(pipe->fragShaderCode, device);

        pipe->device = device;

        return pipe;
    }

    void VulkanPipeline::Free()
    {
        if(vertModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, vertModule, nullptr);
        }

        if(fragModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, fragModule, nullptr);
        }
    }
}