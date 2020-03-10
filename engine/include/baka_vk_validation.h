#ifndef _BAKA_VK_VALIDATION_
#define _BAKA_VK_VALIDATION_

#include <vulkan/vulkan.h>
#include <vector>

namespace baka
{
    class VulkanValidation
    {
    public:
        void Init();
        uint32_t GetLayerCount();
        const char **GetLayerNames();
        void RemoveLayer(const char *name);

    private:
        void QueryLayerProperties();
        std::vector<VkLayerProperties> layers;
        std::vector<const char *> layer_names;
    };

    static VulkanValidation baka_validation;
}

#endif