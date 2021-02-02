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
    //TODO: add staging buffer
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
        m_VertexCount = static_cast<uint32_t>(data->VertexData.size());
        m_VertexBuffer = VertexBuffer::Create(data->VertexData.data(), static_cast<uint32_t>(sizeof(PBRVertex) * data->VertexData.size()));
        m_IndexBuffer = IndexBuffer::Create(data->Indices.data(), static_cast<uint32_t>(data->Indices.size()));

        return true;
    }
}