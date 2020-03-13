#include "baka_logger.h"
#include "baka_vk_swapchain.h"
#include "baka_shader.h"
#include "baka_vk_pipeline.h"
#include "baka_graphics.h"
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
        VkPipelineLayoutCreateInfo pipeLayoutInfo = {};
        VkGraphicsPipelineCreateInfo pipeCreateInfo = {};
        VkResult res;

        VulkanPipeline *pipe = VulkanPipelineNew();
        if(!pipe) return nullptr;

        pipe->vert_shader_code = LoadData(vert);
        pipe->vert_module = CreateModule(pipe->vert_shader_code, device);

        pipe->frag_shader_code = LoadData(frag);
        pipe->frag_module = CreateModule(pipe->frag_shader_code, device);

        pipe->device = device;
        // pipe->descriptor_set_count = count;
        pipe->descriptor_set_count = swap_length;

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

        pipe->CreateDescriptorSetPool();
        pipe->CreateDescriptorSetLayout();
        pipe->CreateDescriptorSets();

        pipeLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeLayoutInfo.setLayoutCount = 1;
        pipeLayoutInfo.pSetLayouts = &pipe->descriptor_set_layout;
        /* push constants: https://stackoverflow.com/questions/50956414/what-is-a-push-constant-in-vulkan
        basically these allow us to pass data to the shaders faster than ubo's, however the amount of data you can send is limited
        and varies depending on the hardware */
        pipeLayoutInfo.pushConstantRangeCount = 0;
        pipeLayoutInfo.pPushConstantRanges = nullptr;

        pipe->SetupRenderPass();

        res = vkCreatePipelineLayout(device, &pipeLayoutInfo, nullptr, &pipe->pipeline_layout);
        if(res != VK_SUCCESS)
        {
            bakaerr("pipeline layout creation failed with error code %d", res);
            pipe->Free();
            return nullptr;
        }

        pipeCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeCreateInfo.stageCount = shaderStages.size();
        pipeCreateInfo.pStages = (VkPipelineShaderStageCreateInfo*)shaderStages.data();
        pipeCreateInfo.pVertexInputState = &vertexInput;
        pipeCreateInfo.pInputAssemblyState = &inputAssembly;
        pipeCreateInfo.pViewportState = &viewportCreateInfo;
        pipeCreateInfo.pRasterizationState = &rasterizerState;
        pipeCreateInfo.pMultisampleState = &multisample;
        pipeCreateInfo.pColorBlendState = &colorBlend;
        pipeCreateInfo.pDynamicState = nullptr;
        pipeCreateInfo.layout = pipe->pipeline_layout;
        pipeCreateInfo.renderPass = pipe->render_pass;
        pipeCreateInfo.subpass = 0;
        pipeCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipeCreateInfo.basePipelineIndex = -1;
        pipeCreateInfo.pDepthStencilState = &depthStencil;

        res = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeCreateInfo, nullptr, &pipe->pipeline);
        if(res != VK_SUCCESS)
        {
            bakaerr("pipeline creation failed with error code %d", res);
            pipe->Free();
            return nullptr;
        }

        return pipe;
    }

    /* need descriptor pools to create descriptor sets 
    helpful link: https://www.reddit.com/r/vulkan/comments/8u9zqr/having_trouble_understanding_descriptor_pool/ 
    CANNOT call this from a thread */
    void VulkanPipeline::CreateDescriptorSetPool()
    {
        VkDescriptorPoolCreateInfo createInfo = {};
        std::vector<VkDescriptorPoolSize> poolSizes(2);
        VkResult res;

        /* pool sizes describe how many descriptors we will need in total for each type of descriptor set */
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = descriptor_set_count;
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = descriptor_set_count;

        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        /* max sets is the total amount of descriptor sets we can allocate from pool */
        createInfo.maxSets = descriptor_set_count;
        /* pool size count is the total number of poolSizes we are passing */
        createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        /* pPoolSizes is a pointer to the first element of the array of poolSizes */
        createInfo.pPoolSizes = (VkDescriptorPoolSize*)poolSizes.data();

        descriptor_pool.resize(1);
        res = vkCreateDescriptorPool(device, &createInfo, nullptr, (VkDescriptorPool*)descriptor_pool.data());
        if(res != VK_SUCCESS)
        {
            bakaerr("descriptor pool creation failed with error code %d", res);
        }

        /** @note we can also create descriptor set pools with different flag bits.
         * one of those is VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
         * this flag bit allows each descriptor set in the descriptor pool to be freed individually, instead of all at once
         */
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
            bakaerr("failed to create descriptor set layer. error code: %d", res);
        }

    }

    /** @note that a single descriptor set can only be used once during each command recording */
    void VulkanPipeline::CreateDescriptorSets()
    {
        VkDescriptorSetAllocateInfo allocInfo = {};
        VkResult res;
        // uint32_t i;
        std::vector<VkDescriptorSetLayout> layouts(descriptor_set_count, descriptor_set_layout);

        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptor_pool[0];
        allocInfo.descriptorSetCount = descriptor_set_count;
        allocInfo.pSetLayouts = (VkDescriptorSetLayout*)layouts.data();

        descriptor_sets.resize(descriptor_set_count);
        res = vkAllocateDescriptorSets(device, &allocInfo, (VkDescriptorSet*)descriptor_sets.data());
        if( res != VK_SUCCESS )
        {
            bakaerr("descriptor set allocation failed with error code %d", res);
        }
    }

    /** 
     * the render pass is where we tell vulkan about the framebuffer attachments we need.
     * this is where we specify depth and color buffers, as well as how many samples to use for each of them
     * and how their contents should be handled during rendering operations
     * https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Render_passes
     */
    void VulkanPipeline::SetupRenderPass()
    {
        VkAttachmentReference depthAttachmentRef = {};
        VkAttachmentDescription depthAttachment = {};
        VkAttachmentReference colorAttachmentRef = {};
        VkAttachmentDescription colorAttachment = {};
        std::vector<VkAttachmentDescription> attachments(2);
        VkSubpassDependency dependency = {};
        VkSubpassDescription subpass = {};
        VkRenderPassCreateInfo createInfo = {};
        VkResult res;

        /* the depth attachment is how we tell vulkan how to calculate what goes on top of what */
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachmentRef.attachment = 1;

        /* tell the attachment description that we are looking for a depth image */
        depthAttachment.format = FindDepthFormat();
        /* only one sample please */
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        /* don't realy care about loading what we had before */
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        /* and we don't care about storing the results of the operation either */
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        /* we don't care what was previously in that image */
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        /* but we are sure that we want a depth image in the end */
        depthAttachment.finalLayout = depthAttachmentRef.layout;

        /** 
         * apparently these describe exectuion and memory dependencies between subpasses:
         * https://vulkan.lunarg.com/doc/view/1.0.37.0/linux/vkspec.chunked/ch07.html
         * https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Rendering_and_presentation
         *  */
        /** 
         * this subpass external refers to the subpass that occurs at the beginning or end of render pass 
         * this depends on if it is specified in srcSubpass or dstSubpass.
         * it's on srcSubpass so it's the one that occurs in the beginning of the renderpass;
         * */
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        /* a 0 means that it is the one we are currently on */
        dependency.dstSubpass = 0;
        /* wait for swapchain to finish reading from image */
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        /* this operation happens in the current subpass */
        dependency.srcAccessMask = 0;
        /* the operation happens during the color attachment subpass */
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        /* which involves reading and writing the color attachment */
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachmentRef.attachment = 0;

        colorAttachment.format = baka_swap.GetChosenFormat().format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        /* we want to clear all colors from the image before we start working on it. it essentially turns all pixel to the clear color */
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        /* store the pixels so we can see it displayed on the screen later */
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        /* have a read about stencil buffers: https://computergraphics.stackexchange.com/questions/12/what-is-a-stencil-buffer */
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        attachments[0] = colorAttachment;
        attachments[1] = depthAttachment;

        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments = (VkAttachmentDescription*)attachments.data();
        createInfo.pSubpasses = &subpass;
        createInfo.subpassCount = 1;
        createInfo.pDependencies = &dependency;
        createInfo.dependencyCount = 1;

        res = vkCreateRenderPass(device, &createInfo, nullptr, &render_pass);
        if( res != VK_SUCCESS )
        {
            bakaerr("render pass failed with error code %d", res);
        }
    }

    VkFormat VulkanPipeline::FindSupportedFormat(std::vector<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        uint32_t i;
        VkFormatProperties props = {0};
        for (i = 0; i < candidates.size();i++)
        {
            vkGetPhysicalDeviceFormatProperties(Graphics::GetDefaultPhysicalDevice(), candidates[i], &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return candidates[i];
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return candidates[i];
            }
        }

        bakawarn("failed to find supported format!");
        return (VkFormat)VK_NULL_HANDLE;
    }

    VkFormat VulkanPipeline::FindDepthFormat()
    {
        /* D stands for depth */
        std::vector<VkFormat> format = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
        return FindSupportedFormat(
            format,
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    void VulkanPipeline::Free()
    {
        uint32_t i;
        for(i = 0; i < descriptor_pool.size(); i++)
        {
            if( descriptor_pool[i] != VK_NULL_HANDLE )
                vkDestroyDescriptorPool(device, descriptor_pool[i], nullptr);
        }

        if(descriptor_set_layout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, descriptor_set_layout, nullptr);
        }

        if(pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(device, pipeline, nullptr);
        }

        if(pipeline_layout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
        }

        if(render_pass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(device, render_pass, nullptr);
        }

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