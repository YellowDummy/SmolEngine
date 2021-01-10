#include "stdafx.h"
#include "Mesh.h"

#include "Core/Renderer/FBXImporter.h"
#include "Core/Renderer/GraphicsPipeline.h"

namespace SmolEngine
{
    bool Mesh::Create(const std::string& filePath, Ref<Mesh>& out_mesh)
    {
        ImportedData* data = new ImportedData();
        if (FBXImporter::Load(filePath, *data))
        {
            out_mesh->m_Pipeline = std::make_shared<GraphicsPipeline>();
            BufferLayout layout(
            {
                { ShaderDataType::Float3, "aPos" },
                { ShaderDataType::Float4, "aColor" }
            });

            struct Vertex
            {
                glm::vec3 pos;
                glm::vec4 color;
            };

            std::vector<Vertex> vertices(data->vertices.size());
            for (uint32_t i = 0; i < data->vertices.size(); ++i)
            {
                vertices[i].pos = data->vertices[i];
                vertices[i].color = data->colors[i];
            }

            VertexBufferCreateInfo vertexBufferCI = {};
            {
                vertexBufferCI.BuffersCount = 1;
                vertexBufferCI.BufferLayot = &layout;
                vertexBufferCI.Sizes = { sizeof(vertices) };
                vertexBufferCI.Vertices = { vertices.data() };
                vertexBufferCI.Stride = sizeof(Vertex);
            }

            IndexBufferCreateInfo indexBufferCI = {};
            {
                indexBufferCI.BuffersCount = 1;
                indexBufferCI.IndicesCounts = { data->indices.size() };
                indexBufferCI.Indices = { data->indices.data() };
            }

            GraphicsPipelineShaderCreateInfo shaderCI = {};
            {
                shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/BaseShader3D_Vulkan_Vertex.glsl";
                shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/BaseShader3D_Vulkan_Fragment.glsl";
                shaderCI.Textures = {  };
            }

            GraphicsPipelineCreateInfo graphicsPipelineCI = {};
            {
                graphicsPipelineCI.IndexBuffer = &indexBufferCI;
                graphicsPipelineCI.VertexBuffer = &vertexBufferCI;
                graphicsPipelineCI.ShaderCreateInfo = &shaderCI;
                graphicsPipelineCI.PipelineName = "MeshPipiline";
            }

            bool result  = out_mesh->m_Pipeline->Create(&graphicsPipelineCI);
            delete data;
            return result;
        }

        return false;
    }
}