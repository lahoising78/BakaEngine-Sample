#ifndef _BAKA_MESH_H_
#define _BAKA_MESH_H_

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include <map>
#include "baka_vectors.h"
#include "baka_matrix.h"

#define BAKA_ATTRIBUTE_DESCRIPTION_COUNT 3
#define MESH_PUSH_CONST_SIZE sizeof(Matrix4)

namespace baka
{
    class Model;

    enum MeshFileType
    {
        MESH_NONE =             0,
        MESH_OBJ =              1,
        MESH_FBX =              2
    };

    typedef struct
    {
        Vec3 vertex;
        Vec3 normal;
        Vec2 texel;
    } Vertex;

    typedef uint32_t Face[3];

    class Mesh
    {
    friend class Model;

    public:
        void Render(Matrix4 mat, VkCommandBuffer cmd);

    private:
        std::vector<Vertex> vertices;
        std::vector<Face> indices;
        const char* name;

        VkBuffer vertex_buffer;
        VkBuffer index_buffer;
    };

    class MeshManager
    {
    friend class Graphics;

    public:
        void Init(uint32_t count);
        const std::vector<VkVertexInputBindingDescription> &GetBindingDescription() { return binding_description; }
        const std::vector<VkVertexInputAttributeDescription> &GetAttributeDescription() { return attribute_description; }
        VkWriteDescriptorSet GetWriteDescriptor() { return write_descriptor_set; }
        size_t GetPushConstantSize() { return MESH_PUSH_CONST_SIZE; }
        Mesh *Load(const char *filename, MeshFileType fileType) { return nullptr; }
        Mesh *Load(std::vector<Vertex> vertices, const char *name) { return nullptr; }
        VkPipelineLayout GetPipeLayout() { return layout; }
        
    private:
        void SetPipeLayout(VkPipelineLayout layout) { this->layout = layout; } 

    private:
        std::vector<Mesh> mesh_list;
        std::vector<VkVertexInputAttributeDescription> attribute_description;
        std::vector<VkVertexInputBindingDescription> binding_description;
        VkWriteDescriptorSet write_descriptor_set;
        std::map<const char *, Mesh*> name_to_mesh;
        VkPipelineLayout layout;

    };

    extern MeshManager baka_mesh;
}

#endif