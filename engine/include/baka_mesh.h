#ifndef _BAKA_MESH_H_
#define _BAKA_MESH_H_

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include "baka_vectors.h"
#include "baka_matrix.h"

#define BAKA_ATTRIBUTE_DESCRIPTION_COUNT 3
#define MESH_PUSH_CONST sizeof(Matrix4)

namespace baka
{
    typedef struct
    {
        Vec3 vertex;
        Vec3 normal;
        Vec2 texel;
    } Vertex;

    typedef uint32_t Face[3];

    class Mesh
    {
    public:
        void Load(const char *filename);
        void Render();

    public:
        std::vector<Vertex> vertices;
        std::vector<Face> indices;
    };

    class MeshManager
    {
    public:
        void Init(uint32_t count);
        const std::vector<VkVertexInputBindingDescription> &GetBindingDescription() { return binding_description; }
        const std::vector<VkVertexInputAttributeDescription> &GetAttributeDescription() { return attribute_description; }
        VkWriteDescriptorSet GetWriteDescriptor() { return write_descriptor_set; }
        size_t GetPushConstantSize() { return MESH_PUSH_CONST; }
        Mesh *Load(const char *filename) { return nullptr; }
        Mesh *Load(std::vector<Vertex> vertices) { return nullptr; }

    private:
        std::vector<Mesh> mesh_list;
        std::vector<VkVertexInputAttributeDescription> attribute_description;
        std::vector<VkVertexInputBindingDescription> binding_description;
        VkWriteDescriptorSet write_descriptor_set;

    };

    extern MeshManager baka_mesh;
}

#endif