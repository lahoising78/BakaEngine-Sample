#include "baka_logger.h"
#include "baka_vk_swapchain.h"
#include "baka_shader.h"
#include "baka_vk_pipeline.h"
#include "baka_mesh.h"
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
        VkPipelineShaderStageCreateInfo vertStageInfo = {};
        VkPipelineShaderStageCreateInfo fragStageInfo = {};
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        VkPipelineVertexInputStateCreateInfo vertexInput = {};
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        VkViewport viewport = {};
        VkRect2D scissor = {};
        VkPipelineViewportStateCreateInfo viewportCreateInfo = {};
        VkPipelineRasterizationStateCreateInfo rasterizerState = {};
        VkPipelineMultisampleStateCreateInfo multisample = {};
        VkPipelineDepthStencilStateCreateInfo depthStencil = {};
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        VkPipelineColorBlendStateCreateInfo colorBlend = {};

        VulkanPipeline *pipe = VulkanPipelineNew();
        if(!pipe) return nullptr;

        pipe->vert_shader_code = LoadData(vert);
        pipe->vert_module = CreateModule(pipe->vert_shader_code, device);

        pipe->frag_shader_code = LoadData(frag);
        pipe->frag_module = CreateModule(pipe->frag_shader_code, device);

        pipe->device = device;

        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;                                   /* less depth value = closer */
        depthStencil.depthBoundsTestEnable = VK_FALSE;                                      /* an aditional optional depth test */
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;                                          /* need a stencil component if we want to enable this */

        vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStageInfo.module = pipe->vert_module;
        vertStageInfo.pName = "main";
        shaderStages.push_back(vertStageInfo);

        fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStageInfo.module = pipe->frag_module;
        fragStageInfo.pName = "main";
        shaderStages.push_back(fragStageInfo);

        /* define what our shaders should be expecting */
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.pVertexBindingDescriptions = (VkVertexInputBindingDescription*)baka_mesh.GetBindingDescription().data();
        vertexInput.vertexBindingDescriptionCount = baka_mesh.GetBindingDescription().size();
        vertexInput.pVertexAttributeDescriptions = (VkVertexInputAttributeDescription*)baka_mesh.GetAttributeDescription().data();
        vertexInput.vertexAttributeDescriptionCount = baka_mesh.GetAttributeDescription().size();

        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;                       /* what are the vertices going to form */
        inputAssembly.primitiveRestartEnable = VK_FALSE;                                    //true to break up lines and triangles when topoly is strip

        /** @note Eventually we should create this with vk pipeline cache to store the general struct of pipeline 
        so we can create pipelines faster based on this one. This could also be used with a future pipeline
        that is created with a derivative bit flag, so it is faster to switch between the pipelines */

        /** @note the viewport tell the pipeline the dst region of the framebuffer where the src image should be renderd */
        viewport.x = 0; viewport.y = 0;                                                         /* render from start of framebuffer */
        viewport.width = (float)extent.width; viewport.height = (float)extent.height;           /* render to the entire framebuffer */
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        /** @note the scissor tells the pipeline the region of the source image to be rendered */
        scissor.offset = {0, 0};
        scissor.extent = extent;                                                                /* render the entire image */

        /** @note you can have multiple viewports and scissors, but that would require a gpu feature to be enabled (an extension) */
        viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportCreateInfo.scissorCount = 1;
        viewportCreateInfo.pScissors = &scissor;
        viewportCreateInfo.viewportCount = 1;
        viewportCreateInfo.pViewports = &viewport;

        rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        /* if true, fragments outside of near and far planes will be clamped to them */
        rasterizerState.depthClampEnable = VK_FALSE;                                            
        /* allow primitives to reach fragment shader stage. why would you want to turn this on? https://stackoverflow.com/questions/42470669/when-does-it-make-sense-to-turn-off-the-rasterization-step */
        rasterizerState.rasterizerDiscardEnable = VK_FALSE;                                     
        /* fill are of polygon. you could alse set this to line and point. anything other than fill requires a gpu feature */
        rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;                                     
        /* how many fragments of thickness should a line have? anything bigger than 1.0f requires a gpu feature */
        rasterizerState.lineWidth = 1.0f;                                                       
        /* google it */
        rasterizerState.cullMode = VK_CULL_MODE_BACK_BIT;                                       
        /* what direction we are expecting the vertices */
        rasterizerState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;                            
        /* can be used to alter depth. useful for shadow mapping */
        rasterizerState.depthBiasEnable = VK_FALSE;                                             
        rasterizerState.depthBiasConstantFactor = 0.0f;
        rasterizerState.depthBiasClamp = 0.0f;
        rasterizerState.depthBiasSlopeFactor = 0.0f;

        /* this multisampling, obviously. Can be used for anti-aliasing */
        multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample.sampleShadingEnable = VK_FALSE;
        multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisample.minSampleShading = 1.0f;
        multisample.pSampleMask = nullptr;
        multisample.alphaToCoverageEnable = VK_FALSE;
        multisample.alphaToOneEnable = VK_FALSE;

        /* this one is per frame buffer */
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        /* this one is global color blending settings */
        colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlend.logicOpEnable = VK_FALSE;                                            /** @note if true, process as if blendEnable was false in attachment */
        colorBlend.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlend.attachmentCount = 1;
        colorBlend.pAttachments = &colorBlendAttachment;
        colorBlend.blendConstants[0] = 0.0f; // Optional
        colorBlend.blendConstants[1] = 0.0f; // Optional
        colorBlend.blendConstants[2] = 0.0f; // Optional
        colorBlend.blendConstants[3] = 0.0f; // Optional

        pipe->CreateDescriptorSetLayout();

        return pipe;
    }

    void VulkanPipeline::CreateDescriptorSetLayout()
    {
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings(2);
        VkDescriptorSetLayoutCreateInfo createInfo = {};
        VkResult res;

        /* ubo is a buffer that contains any information you want */
        layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;   
        layoutBindings[0].binding = 0;
        /** @note can use multiple ubo's for skeletal animation. 
        each bone would contain a ubo */
        layoutBindings[0].descriptorCount = 1;                                  
        /* can use this to set target stage */
        layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        layoutBindings[0].pImmutableSamplers = nullptr;

        /* this is what gets the texture to the shader */ 
        layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layoutBindings[1].binding = 1;
        layoutBindings[1].descriptorCount = 1;
        layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        /* set this to point to an array of samplers if you want to permanently bind the vk sampler to the layout */
        layoutBindings[1].pImmutableSamplers = nullptr;

        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = layoutBindings.size();
        createInfo.pBindings = (VkDescriptorSetLayoutBinding*)layoutBindings.data();

        res = vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &descriptor_set_layout);
        if(res != VK_SUCCESS)
        {
            bakaerr("failed to create descriptor set layer. error code: %u", res);
        }

    }

    void VulkanPipeline::Free()
    {
        if(vert_module != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, vert_module, nullptr);
        }

        if(frag_module != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, frag_module, nullptr);
        }
    }
}