#ifndef _BAKA_SHADER_H_
#define _BAKA_SHADER_H_

#include <vulkan/vulkan.h>
#include <vector>

namespace baka
{
    VkShaderModule CreateModule(const std::vector<char> &data, VkDevice device);
    std::vector<char> LoadData(const char *filename);
}

#endif