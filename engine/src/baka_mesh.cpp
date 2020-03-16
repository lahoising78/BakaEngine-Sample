#include "baka_logger.h"
#include "baka_mesh.h"
#include "baka_vk_pipeline.h"
#include "baka_graphics.h"

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

    void Mesh::Render(Matrix4 mat)
    {
        VkCommandBuffer cmd = Graphics::GetCurrentCommandBuffer();

        vkCmdPushConstants(
            cmd, baka_mesh.GetPipeLayout(),
            VK_SHADER_STAGE_VERTEX_BIT,
            0, MESH_PUSH_CONST_SIZE, mat
        );

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(cmd, 0, 1, &vertex_buffer, offsets );
        vkCmdBindIndexBuffer(cmd, index_buffer, 0, VkIndexType::VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(cmd, indices.size() * 3, 1, 0, 0, 0);
    }
}