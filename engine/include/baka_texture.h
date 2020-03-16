#ifndef _BAKA_TEXTURE_H_
#define _BAKA_TEXTURE_H_

#include <vulkan/vulkan.h>
#include <vector>

namespace baka
{
    class Model;

    class Texture
    {
    friend class Model;
    private:
        void BindDescriptorSet(VkCommandBuffer cmd);

    private:
        VkImageView image_view;
        VkSampler image_sampler;
        const char *texture_name;
        VkDescriptorSet descriptor_set;
        uint32_t descriptor_set_id;

    };

    class TextureManager
    {
    friend class Graphics;
    public:
        Texture *Load(const char *filename) { return nullptr; }
        VkPipelineLayout GetPipelineLayout() { return pipe_layout; }
        
    private:
        void Init(uint32_t count);
        void SetPipelineLayout(VkPipelineLayout layout) { pipe_layout = layout; }
    
    private:
        std::vector<Texture> texture_list;
        VkPipelineLayout pipe_layout;
        VkDevice device;
    };

    extern TextureManager baka_texture_manager;
}

#endif