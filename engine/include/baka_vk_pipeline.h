#ifndef _BAKA_VK_PIPELINE_H_
#define _BAKA_VK_PIPELINE_H_

#include <vulkan/vulkan.h>
#include <vector>

#define MANAGER_NEW(type,manager,var) type *manager::type##New() {\
    for(uint32_t i = 0; i < var.size(); i++) {\
        if(var[i].inuse) continue; var[i].inuse = true; return &var[i];\
    }\
    return nullptr;\
}

// #define MANAGER_FOREACH(var, body) for( uint32_t i = 0; i < var.size(); i++ ) { body }

namespace baka
{
    class VulkanPipeline
    {
    private:
        void Free();

    private:
        std::vector<char> vertShaderCode;
        std::vector<char> fragShaderCode;

        VkShaderModule vertModule;
        VkShaderModule fragModule;

        bool inuse;
        friend class VulkanPipelineManager;
    };

    class VulkanPipelineManager
    {
    public:
        void Init(uint32_t count);
        VulkanPipeline *CreateBasicModel(VkDevice device, const char *vert, const char *frag, VkExtent2D extent, uint32_t count);
        void Close();
        VkDevice GetDevice() {return device;}

    private:
        VulkanPipeline *VulkanPipelineNew();

    private:
        std::vector<VulkanPipeline> pipeline_list;
        uint32_t swap_length;
        VkDevice device;
    };

    extern VulkanPipelineManager baka_pipeline_manager;
}

#endif