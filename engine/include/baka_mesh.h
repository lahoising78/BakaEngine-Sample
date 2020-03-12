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
        const std::vector<VkVertexInputBindingDescription> &GetBindingDescription() { return binding_description; }
        const std::vector<VkVertexInputAttributeDescription> &GetAttributeDescription() { return attribute_description; }

    private:
        std::vector<Mesh> mesh_list;
        std::vector<VkVertexInputAttributeDescription> attribute_description;
        std::vector<VkVertexInputBindingDescription> binding_description;

    };

    extern MeshManager baka_mesh;
}

#endif