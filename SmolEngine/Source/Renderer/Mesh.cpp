#include "stdafx.h"
#include "Mesh.h"

#include "Utils/ModelImporter.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/SharedUtils.h"
#include "ECS/Systems/CommandSystem.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
    Ref<Mesh> Mesh::Create(const std::string& filePath)
    {
        Ref<Mesh> mesh = nullptr;
        ImportedData* data = new ImportedData();
        if (ModelImporter::Load(filePath, data))
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
        m_VertexCount = static_cast<uint32_t>(data->vertices.size());

        std::vector<PBRVertex> vertices(data->vertices.size());
        for (uint32_t i = 0; i < data->vertices.size(); ++i)
        {
            vertices[i].Pos = data->vertices[i];
            vertices[i].Color = data->colors[i];
            vertices[i].Normals = data->normals[i];
            vertices[i].Tangent = glm::vec4(data->tangents[i], 1);
            vertices[i].UVs = data->uvs[i];
        }

        m_VertexBuffer = VertexBuffer::Create(vertices.data(), static_cast<uint32_t>(sizeof(PBRVertex) * vertices.size()));
        m_IndexBuffer = IndexBuffer::Create(data->indices.data(), static_cast<uint32_t>(data->indices.size()));
        return true;
    }
}