#include "baka_logger.h"
#include "baka_vk_extensions.h"
#include <algorithm>
#include <string.h>

namespace Baka
{
    void VulkanInstanceExtensions::Init()
    {
        uint32_t extensionCount;
        extension_names.clear();
        extensions.clear();

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        bakalog("Available instance extensions: %u", extensionCount);
        if(!extensionCount) return;

        extensions.resize(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        for(const auto &extension : extensions)
        {
            // extension_names.push_back(extension.extensionName);
            bakalog("\t%s", extension.extensionName);
        }
    }

    void VulkanDeviceExtensions::Init()
    {

    }

    bool VulkanExtensions::EnableExtension(const char *name)
    {
        bakalog("Enable extension %s", name);

        if(std::find(extension_names.begin(), extension_names.end(), name) != extension_names.end())
        {
            bakalog("Extension %s has already been enabled", name);
            return false;
        }

        if( !ExtensionAvailable(name) ) 
        {
            bakawarn("Extension %s is not available", name);
            return false;
        }

        extension_names.push_back(name);
        return true;
    }

    bool VulkanExtensions::ExtensionAvailable(const char *name)
    {
        for(VkExtensionProperties &extension : extensions )
        {
            if( strcmp(extension.extensionName, name) == 0 )
                return true;
        }

        return false;
    }

    const char **VulkanExtensions::GetExtensionNames()
    {
        return extension_names.data();
    }

    uint32_t VulkanExtensions::GetExtensionCount()
    {
        return extension_names.size();
    }
}