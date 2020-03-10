#include "baka_logger.h"
#include "baka_mesh.h"

namespace baka
{
    MeshManager baka_mesh;

    void MeshManager::Init(uint32_t count)
    {
        bakalog("initialize mesh manager");
        if(!count)
        {
            bakawarn("cannot initialize mesh manager with 0 elements");
            return;
        }
        mesh_list.resize(count);

        binding_description.binding = 0;
        binding_description.stride = sizeof(Vertex);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        attribute_description[0].binding = 0;
        attribute_description[0].location = 0;
        attribute_description[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_description[0].offset = offsetof(Vertex, vertex);

        attribute_description[1].binding = 0;
        attribute_description[1].location = 1;
        attribute_description[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_description[1].offset = offsetof(Vertex, normal);

        attribute_description[2].binding = 0;
        attribute_description[2].location = 2;
        attribute_description[2].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_description[2].offset = offsetof(Vertex, texel);
    }
}