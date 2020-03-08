#ifndef _BAKA_VK_VALIDATION_
#define _BAKA_VK_VALIDATION_

#include <vulkan/vulkan.h>
#include <vector>

namespace Baka
{
    class VulkanValidation
    {
    public:
        void Init();
        const std::vector<const char *> GetLayerNames();

    private:
        std::vector<VkLayerProperties> availableLayer;
        std::vector<const char *> layer_names;

    };

    static VulkanValidation validation;
}

#endif