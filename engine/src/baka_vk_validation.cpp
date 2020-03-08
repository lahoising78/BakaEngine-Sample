#include <algorithm>
#include <string.h>
#include "baka_logger.h"
#include "baka_vk_validation.h"

namespace Baka
{
    void VulkanValidation::Init()
    {
        QueryLayerProperties();
    }

    void VulkanValidation::QueryLayerProperties()
    {
        uint32_t layerCount = 0;
        
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        bakalog("Layer Count %u", layerCount);

        layers.resize(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

        bakalog("Available validation layers: ");
        for(const auto &layer : layers)
        {
            layer_names.push_back(layer.layerName);
            bakalog("\t%s", layer.layerName);
        }
    }

    uint32_t VulkanValidation::GetLayerCount()
    {
        return layer_names.size();
    }

    const char **VulkanValidation::GetLayerNames()
    {
        return layer_names.data();
    }

    void VulkanValidation::RemoveLayer(const char *name)
    {
        uint32_t i;

        for( i = 0; i < layer_names.size(); i++)
        {
            if( strcmp( layer_names[i], name ) == 0 )
            {
                bakawarn("Remove layer %s", name);
                layer_names.erase(layer_names.begin() + i);
                break;
            }
        }
    }
}