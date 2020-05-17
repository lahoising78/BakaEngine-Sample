#ifndef _VK_SAMPLE_H_
#define _VK_SAMPLE_H_

#include "baka_vulkan.h"
#include "baka_logger.h"

class SampleVK : public baka::VulkanGraphics
{
public:
    SampleVK(const char *applicationName) : baka::VulkanGraphics(applicationName) {}
    ~SampleVK() {}

protected:
    void CreateInstance() override
    {
        bakalog("I'm %s's child", applicationName);
    }
};

#endif