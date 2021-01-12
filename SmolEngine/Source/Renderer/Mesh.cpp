#include "stdafx.h"
#include "Mesh.h"

#include "Utils/FBXImporter.h"
#include "Renderer/GraphicsPipeline.h"
#include "ECS/Systems/CommandSystem.h"

namespace SmolEngine
{
    bool Mesh::Create(const std::string& filePath, Ref<Mesh>& out_mesh)
    {
        ImportedData* data = new ImportedData();
        bool create_result = false;
        if (FBXImporter::Load(filePath, data))
            create_result = out_mesh->Init(data);

        delete data;
        return create_result;
    }

    void Mesh::Free()
    {
        if(m_Pipeline)
            m_Pipeline->Destroy();
    }

    bool Mesh::Init(ImportedData* data)
    {
        Free();
        m_VertexCount = data->vertices.size();
        m_Pipeline = std::make_shared<GraphicsPipeline>();
        BufferLayout layout(
            {
                { ShaderDataType::Float3, "aPos" },
                { ShaderDataType::Float3, "aNormal" },
                { ShaderDataType::Float2, "aUV" },
                { ShaderDataType::Float4, "aColor" }
            });

        struct Vertex
        {
            glm::vec3 pos;
            glm::vec3 normals;
            glm::vec2 uvs;
            glm::vec4 color;
        };

        glm::vec3 pos(0, 0, 0);
        glm::vec3 scale(1, 1, 1);
        glm::vec3 rot(0, 0, 0);
        glm::mat4 transform;

        CommandSystem::ComposeTransform(pos, rot, scale, true, transform);

        std::vector<Vertex> vertices(data->vertices.size());
        for (uint32_t i = 0; i < data->vertices.size(); ++i)
        {
            vertices[i].pos = glm::vec4(data->vertices[i], 1) * transform;
            vertices[i].color = data->colors[i];
            vertices[i].normals = data->normals[i];
            vertices[i].uvs = data->uvs[i];
        }

        VertexBufferCreateInfo vertexBufferCI = {};
        {
            vertexBufferCI.BuffersCount = 1;
            vertexBufferCI.BufferLayot = &layout;
            vertexBufferCI.Sizes = { sizeof(Vertex) * vertices.size() };
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

        return m_Pipeline->Create(&graphicsPipelineCI);
    }
}