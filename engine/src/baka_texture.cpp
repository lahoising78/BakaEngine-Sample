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
}