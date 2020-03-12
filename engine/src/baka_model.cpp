#include "baka_logger.h"
#include "baka_model.h"
#include "baka_graphics.h"

namespace baka
{
    ModelManager baka_model_manager = {};

    void ModelManager::Init(uint32_t count, uint32_t swapLength, VkDevice device)
    {
        bakalog("initializing model manager");

        if(!count)
        {
            bakawarn("cannot init model manager with 0 elements");
            return;
        }
        model_list.resize(count);

        this->swap_length = swapLength;
        this->device = device;
        default_pipe = Graphics::GetDefaultPipeline();
    }
}