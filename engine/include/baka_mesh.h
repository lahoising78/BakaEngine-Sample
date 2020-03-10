#ifndef _BAKA_MESH_H_
#define _BAKA_MESH_H_

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include "baka_vectors.h"

#define BAKA_ATTRIBUTE_DESCRIPTION_COUNT 3

namespace baka
{
    typedef struct
    {
        Vec3 vertex;
        Vec3 normal;
        Vec2 texel;
    } Vertex;

    typedef struct
    {
        uint32_t vertices[3];
    } Face;

    class Mesh
    {

    };

    class MeshManager
    {
    public:
        void Init(uint32_t count);

    private:
        std::vector<Mesh> mesh_list;
        VkVertexInputAttributeDescription attribute_description[BAKA_ATTRIBUTE_DESCRIPTION_COUNT];
        VkVertexInputBindingDescription binding_description;

    };

    extern MeshManager baka_mesh;
}

#endif