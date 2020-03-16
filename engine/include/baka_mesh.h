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
    class MeshManager;

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

    class Mesh
    {
    friend class Model;
    friend class MeshManager;

    public:
        ~Mesh();
        void Render(Matrix4 mat, VkCommandBuffer cmd);
        void Free();

    private:
        void Setup(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
        void CreateVertexBuffer();
        void CreateIndexBuffer();

    private:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::string name;

        VkBuffer vertex_buffer;
        VkDeviceMemory vertex_buffer_memory;
        VkBuffer index_buffer;
        VkDeviceMemory index_buffer_memory;

        bool inuse;
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
        Mesh *Load(std::vector<Vertex> vertices, std::vector<uint32_t> faces, const char *name);
        VkPipelineLayout GetPipeLayout() { return layout; }
        
    private:
        void SetPipeLayout(VkPipelineLayout layout) { this->layout = layout; } 
        Mesh *MeshNew();

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