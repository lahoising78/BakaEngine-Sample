#ifndef _BAKA_VK_EXTENSIONS_
#define _BAKA_VK_EXTENSIONS_

#include <vulkan/vulkan.h>
#include <vector>

namespace baka
{
    class VulkanExtensions
    {
    public:
        virtual void Init(VkPhysicalDevice device = {}) {};
        const char **GetExtensionNames();
        uint32_t GetExtensionCount();
        bool EnableExtension(const char *name);
    protected:
        std::vector<VkExtensionProperties> extensions;
        std::vector<const char *> extension_names;
    private:
        bool ExtensionAvailable(const char *name); 
    };

    class VulkanInstanceExtensions : public VulkanExtensions
    {
    public:
        void Init(VkPhysicalDevice device = {}) override;
    };

    class VulkanDeviceExtensions : public VulkanExtensions
    {
    public:
        void Init(VkPhysicalDevice device) override;
    };

    static VulkanInstanceExtensions instance_extensions;
    static VulkanDeviceExtensions device_extensions;
}

#endif