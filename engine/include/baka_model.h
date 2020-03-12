#ifndef _BAKA_MODEL_H_
#define _BAKA_MODEL_H_

#include "baka_vk_pipeline.h"
#include "baka_mesh.h"
#include <vector>

namespace baka
{
    class Model
    {
    public:
        Mesh mesh;
    };

    class ModelManager
    {
    public:
        void Init(uint32_t count, uint32_t swapLength, VkDevice device);

    private:
        std::vector<Model> model_list;
        VkDevice device;
        uint32_t swap_length;
        VulkanPipeline *default_pipe;
    };

    extern ModelManager baka_model_manager;
}

#endif