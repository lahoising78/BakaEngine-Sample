#include "baka_logger.h"
#include "baka_vk_command.h"

namespace baka
{
    /* Multi threaded commands: https://stackoverflow.com/questions/38318818/multi-thread-rendering-vs-command-pools */

    /** 
     * to draw in vulkan you record the draw calls and operations using command buffers. 
     * this allows us to do all the hard work in advance and/or in multiple threads
     * https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Command_buffers
     * */
    VulkanCommand::VulkanCommand(VkDevice device, uint32_t bufferCount, uint32_t queueFamIndex)
    {
        VkResult res;
        VkCommandPoolCreateInfo poolInfo = {};
        VkCommandBufferAllocateInfo bufferAllocInfo = {};

        this->device = device;

        /* a command buffer cannot be created by itself. it must be created from a command pool */
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamIndex;
        /* for now */
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        // poolInfo.flags = 0;

        res = vkCreateCommandPool(device, &poolInfo, nullptr, &command_pool);
        if(res != VK_SUCCESS)
        {
            bakaerr("command pool creation failed with error code %d", res);
            return;
        }

        command_buffers.resize(bufferCount);
        bufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        /** 
         * primary lets us submit commands for executions, but cannot be called from another command buffer
         * secondary is the complete opposite. this allows us to do reuse common operations from other command buffers
         * */
        bufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        bufferAllocInfo.commandPool = command_pool;
        bufferAllocInfo.commandBufferCount = (uint32_t)command_buffers.size();

        res = vkAllocateCommandBuffers(device, &bufferAllocInfo, (VkCommandBuffer*)command_buffers.data());
        if(res != VK_SUCCESS)
        {
            bakaerr("command buffer allocation failed with error code %d", res);
            Free();
            return;
        }

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        // fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        fences.resize(bufferCount);
        for(auto &f : fences)
        {
            VkResult res;
            res = vkCreateFence(device, &fenceInfo, nullptr, &f);
            if(res != VK_SUCCESS)
            {
                bakaerr("command buffer fence creation failed");
            }
        }

        buffer_submitted.resize(bufferCount, false);
        current_buf = 0;

        bakalog("created command pool and buffers");
    }

    void VulkanCommand::WaitFence()
    {
        VkResult res;
        /* when you submit things to the queue for presentation and stuff, they perform those jobs asynchronously.
        before we can work with the command buffers, we must make sure that it is not currently being use.
        we do this by setting fences. these are signals that we can set and wait for */
        if( buffer_submitted[current_buf] )
        {
            res = vkWaitForFences( device, 1, &fences[current_buf], VK_TRUE, UINT64_MAX );
            if(res != VK_SUCCESS)
            {
                bakawarn("wait for fence error code %d", res);
            }
            buffer_submitted[current_buf] = false;
        }

        /* after you wait for the fences, you must manually put them back to the non-signaled state */
        res = vkResetFences(device, 1, &fences[current_buf]);
        if(res != VK_SUCCESS)
        {
            bakawarn("fence reset error %d", res);
        }
    }

    /* start recording command so we can pass them to the queue in the end */
    VkResult VulkanCommand::BeginCmdBuffer()
    {
        VkResult res;
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        WaitFence();

        /* begin recording */
        res = vkBeginCommandBuffer(command_buffers[current_buf], &beginInfo);
        if(res != VK_SUCCESS)
        {
            bakaerr("error intializing command buffer");
        }

        return res;
    }

    VkResult VulkanCommand::SubmitQueue( VkQueue queue, std::vector<VkSubmitInfo> &submitInfo, bool wait_fence )
    {
        VkResult res;

        /* submit command buffer to the queue */
        if(wait_fence)
        {
            /* the fence is the fence that we want to notify of the completion */
            res = vkQueueSubmit(queue,submitInfo.size(), submitInfo.data(), fences[current_buf]);
            buffer_submitted[current_buf] = true;
        }
        else
        {
            res = vkQueueSubmit(queue, submitInfo.size(), submitInfo.data(), VK_NULL_HANDLE);
        }

        if(res !=VK_SUCCESS)
        {
            bakaerr("queue submition failed with error code %d", res);
        }

        return res;
    }

    void VulkanCommand::Free()
    {
        for(auto &f : fences)
        {
            if(f != VK_NULL_HANDLE)
            {
                vkDestroyFence(device, f, nullptr);
            }
        }

        if(command_buffers.size() > 0 && command_buffers[0] != VK_NULL_HANDLE)
            vkFreeCommandBuffers(device, command_pool, command_buffers.size(), command_buffers.data());
        
        if(command_pool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(device, command_pool, nullptr);
        }

    }

    VkResult VulkanCommand::InitDefaultCommand(VkRenderPass renderpass, VkExtent2D extent, std::vector<VkFramebuffer> framebuffers)
    {
        VkCommandBufferBeginInfo cmdBufInfo = {};
        cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkClearValue clearValues[2];
		clearValues[0].color = {0.8f, 0.4f, 0.4f, 1.0f};
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderpass;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width =  extent.width;
		renderPassBeginInfo.renderArea.extent.height = extent.height;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;

        for (uint32_t i = 0; i < command_buffers.size(); ++i)
		{
			renderPassBeginInfo.framebuffer = framebuffers[i];

			VkResult res = vkBeginCommandBuffer(command_buffers[i], &cmdBufInfo);
            if(res != VK_SUCCESS)
            {
                bakaerr("failed to begin command buffer with error code %d", res);
                return res;
            }

			vkCmdBeginRenderPass(command_buffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdEndRenderPass(command_buffers[i]);

			res = vkEndCommandBuffer(command_buffers[i]);
            if(res != VK_SUCCESS)
            {
                bakaerr("failed to end cmd buffer with error code %d", res);
                return res;
            }
		}

        return VK_SUCCESS;
    }
}