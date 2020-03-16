#include "baka_logger.h"
#include "baka_graphics.h"
#include "baka_texture.h"

namespace baka
{
    TextureManager baka_texture_manager = {};

    void TextureManager::Init(uint32_t count)
    {
        bakalog("initialize texture manager");
        if(!count) 
        {
            bakawarn("cannot initialize texture manager with 0 textures");
            return;
        }
        baka_texture_manager.texture_list.resize(count);
        device = Graphics::GetDefaultLogicalDevice();
    }

    void Texture::BindDescriptorSet(VkCommandBuffer cmd)
    {
        vkCmdBindDescriptorSets(
            cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, 
            baka_texture_manager.GetPipelineLayout(),
            0, 1, &descriptor_set,
            0, nullptr
        );
    }
}