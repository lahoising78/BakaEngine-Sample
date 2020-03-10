#ifndef _BAKA_TEXTURE_H_
#define _BAKA_TEXTURE_H_

#include <vulkan/vulkan.h>
#include <vector>

namespace baka
{
    class Texture
    {

    };

    class TextureManager
    {
    public:
        void Init(uint32_t count);
    
    private:
        std::vector<Texture> texture_list;
        VkDevice device;
    };

    extern TextureManager baka_texture_manager;
}

#endif