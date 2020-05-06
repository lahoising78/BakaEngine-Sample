#include "baka_logger.h"
#include "baka_mesh.h"
#include "baka_vk_pipeline.h"
#include "baka_graphics.h"
#include "baka_vk_utils.h"

namespace baka
{
    MeshManager baka_mesh;

    /* --============= MESH MANAGER =============-- */
    void MeshManager::Init(uint32_t count)
    {
        bakalog("initialize mesh manager");
        if(!count)
        {
            bakawarn("cannot initialize mesh manager with 0 elements");
            return;
        }
        mesh_list.resize(count);
        bakalog("size of mesh list is %u", mesh_list.size());

        binding_description.resize(1);
        binding_description[0].binding = 0;
        binding_description[0].stride = sizeof(Vertex);
        binding_description[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        attribute_description.resize(BAKA_ATTRIBUTE_DESCRIPTION_COUNT);
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

        write_descriptor_set = {};
        // write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        // write_descriptor_set.dstBinding = DESCRIPTOR_UBO_BINDING;
        // write_descriptor_set.dstArrayElement = 0;
        // write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // write_descriptor_set.descriptorCount = 1;
    }

    MANAGER_NEW(Mesh, MeshManager, mesh_list)

    Mesh *MeshManager::Load(std::vector<Vertex> vertices, std::vector<uint32_t> faces, const char *name)
    {
        Mesh *mesh = nullptr;

        if(name_to_mesh[name]) return name_to_mesh[name];
        
        mesh = MeshNew();
        if(!mesh)
        {
            bakawarn("no more meshes available");
            return nullptr;
        }
        name_to_mesh[name] = mesh;
        mesh->name = name;

        return mesh;
    }

    /* --========== MESH ==========-- */
    Mesh::~Mesh()
    {
        Free();
    }

    void Mesh::Free()
    {
        VkDevice device = Graphics::GetDefaultLogicalDevice();

        if(vertex_buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, vertex_buffer, nullptr);
        }

        if(index_buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, index_buffer, nullptr);
        }

        inuse = false;
    }

    void Mesh::Setup(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    {
        this->vertices = vertices;
        this->indices = indices;

        CreateVertexBuffer();
        CreateIndexBuffer();
    }

    void Mesh::CreateVertexBuffer()
    {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VkDevice device = Graphics::GetDefaultLogicalDevice();
        VkDeviceSize s = sizeof(Vertex) * vertices.size();
        
        CreateBuffer( 
            s,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory
        );

        void *data;
        vkMapMemory(device, stagingBufferMemory, 0, s, 0, &data);
            memcpy(data, vertices.data(), (size_t)s);
        vkUnmapMemory(device, stagingBufferMemory);

        CreateBuffer(
            s,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertex_buffer, vertex_buffer_memory
        );

        CopyBuffer(stagingBuffer, vertex_buffer, s);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);

        bakalog("created mesh vertex buffers");
    }

    void Mesh::CreateIndexBuffer()
    {
        VkBuffer stagingBuffer = {};
        VkDeviceMemory stagingBufferMemory = {};
        VkDevice device = Graphics::GetDefaultLogicalDevice();
        VkDeviceSize s = sizeof( indices[0] ) * indices.size();

        CreateBuffer( 
            s, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory 
        );

        void *data;
        vkMapMemory(device, stagingBufferMemory, 0, s, 0, &data);
            memcpy(data, indices.data(), s);
        vkUnmapMemory(device, stagingBufferMemory);

        CreateBuffer(
            s,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            index_buffer, index_buffer_memory
        );

        CopyBuffer(stagingBuffer, index_buffer, s);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, index_buffer_memory, nullptr);
    }

    void Mesh::Render(Matrix4 mat, VkCommandBuffer cmd)
    {
        vkCmdPushConstants(
            cmd, baka_mesh.GetPipeLayout(),
            VK_SHADER_STAGE_VERTEX_BIT,
            0, MESH_PUSH_CONST_SIZE, mat
        );

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(cmd, 0, 1, &vertex_buffer, offsets );
        vkCmdBindIndexBuffer(cmd, index_buffer, 0, VkIndexType::VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(cmd, indices.size(), 1, 0, 0, 0);
    }
}