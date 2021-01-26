#include "stdafx.h"
#include "Mesh.h"

#include "Utils/FBXImporter.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "ECS/Systems/CommandSystem.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
    Ref<Mesh> Mesh::Create(const std::string& filePath)
    {
        Ref<Mesh> mesh = nullptr;
        ImportedData* data = new ImportedData();
        if (FBXImporter::Load(filePath, data))
        {
            mesh = std::make_shared<Mesh>();
            mesh->Init(data);
        }

        delete data;
        return mesh;
    }

    void Mesh::Free()
    {
        if (m_IndexBuffer)
            m_IndexBuffer->Destory();
        if (m_VertexBuffer)
            m_VertexBuffer->Destory();
    }

    bool Mesh::Init(ImportedData* data)
    {
        Free();
        struct Vertex
        {
            glm::vec3 pos;
            glm::vec3 normals;
            glm::vec4 tangent;
            glm::vec2 uvs;
            glm::vec4 color;
        };

        m_Stride = sizeof(Vertex);
        m_VertexCount = static_cast<uint32_t>(data->vertices.size());
        m_Layout =
        {
            { ShaderDataType::Float3, "aPos" },
            { ShaderDataType::Float3, "aNormal" },
            { ShaderDataType::Float4, "aTangent" },
            { ShaderDataType::Float2, "aUV" },
            { ShaderDataType::Float4, "aColor" }
        };

        std::vector<Vertex> vertices(data->vertices.size());
        for (uint32_t i = 0; i < data->vertices.size(); ++i)
        {
            vertices[i].pos = data->vertices[i];
            vertices[i].color = data->colors[i];
            vertices[i].normals = data->normals[i];
            vertices[i].tangent = glm::vec4(data->tangents[i], 1);
            vertices[i].uvs = data->uvs[i];
        }

        m_VertexBuffer = VertexBuffer::Create(vertices.data(), static_cast<uint32_t>(sizeof(Vertex) * vertices.size()));
        m_IndexBuffer = IndexBuffer::Create(data->indices.data(), static_cast<uint32_t>(data->indices.size()));
        return true;
    }
}