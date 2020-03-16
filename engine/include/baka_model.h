#ifndef _BAKA_MODEL_H_
#define _BAKA_MODEL_H_

#include "baka_vk_pipeline.h"
#include "baka_mesh.h"
#include "baka_texture.h"
#include "baka_matrix.h"
#include <vector>

namespace baka
{
    class Model
    {
    public:
        Model() {}
        Model(Mesh *mesh, Texture *texture);
        Model(Mesh *mesh, const char *texturefile);
        Model(const char *meshfile, Texture *texture);
        Model(const char *meshfile, const char *texturefile);
        void Render(Matrix4 mat);

    private:
        Mesh *mesh;
        Texture *texture;
    
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