#include <fstream>
#include "baka_logger.h"
#include "baka_shader.h"

namespace baka
{
    VkShaderModule CreateModule(const std::vector<char> &data, VkDevice device)
    {
        VkShaderModule module = {};
        VkShaderModuleCreateInfo createInfo = {};
        
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = data.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(data.data());
        
        if (vkCreateShaderModule(device, &createInfo, NULL, &module) != VK_SUCCESS)
        {
            bakalog("failed to create shader module");
        }
        return module;
    }

    std::vector<char> LoadData(const char *filename)
    {
        std::vector<char> data;
        size_t fileSize;

        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if(!file.is_open())
        {
            bakawarn("unable to open file %s", filename);
            return data;
        }

        fileSize = (size_t) file.tellg();
        data.resize(fileSize);

        file.seekg(0);
        file.read(data.data(), fileSize);
        
        file.close();

        return data;
    }
}