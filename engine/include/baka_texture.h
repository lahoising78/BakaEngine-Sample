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
        VkImageView image_view;
        VkSampler image_sampler;
        const char *texture_name;

    };

    class TextureManager
    {
    public:
        void Init(uint32_t count);
        Texture *Load(const char *filename) { return nullptr; }
    
    private:
        std::vector<Texture> texture_list;
        VkDevice device;
    };

    extern TextureManager baka_texture_manager;
}

#endif