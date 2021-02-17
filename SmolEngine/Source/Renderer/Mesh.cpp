#include "stdafx.h"
#include "Mesh.h"

#include "Utils/ModelImporter.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Material.h"
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

    Ref<Mesh> Mesh::FindSubMeshByIndex(uint32_t index)
    {
        if (index >= m_SubMeshes.size())
            return nullptr;

        return m_SubMeshes[index];
    }

    Ref<Mesh> Mesh::FindSubMeshByName(const std::string& name)
    {
        for (auto& sub : m_SubMeshes)
        {
            if (sub->m_Name == name)
                return sub;
        }

        return nullptr;
    }

    void Mesh::SetMaterialID(int32_t ID, bool submeshes)
    {
        m_MaterialID = ID;
        if (submeshes)
        {
            for (auto& mesh : m_SubMeshes)
            {
                mesh->SetMaterialID(ID);
            }
        }
    }

    void Mesh::Free()
    {
        if (m_IndexBuffer)
            m_IndexBuffer->Destory();
        if (m_VertexBuffer)
            m_VertexBuffer->Destory();

        for (auto& subMeshes: m_SubMeshes)
            subMeshes->Free();

        if (m_SubMeshes.size() != 0)
            m_SubMeshes.clear();
    }

    bool Mesh::Init(ImportedData* data)
    {
        Free();
        if (data->Components.size() > 1)
            m_SubMeshes.reserve(data->Components.size() - 1);

        for (uint32_t i = 0; i < static_cast<uint32_t>(data->Components.size()); ++i)
        {
            auto& component = data->Components[i];

            // Main
            if (i == 0)
            {
                CreateVertexAndIndexBuffers(component);
                continue;
            }

            // Sub
            Ref<Mesh> mesh = std::make_shared<Mesh>();
            mesh->CreateVertexAndIndexBuffers(component);
            m_SubMeshes.emplace_back(mesh);
        }

        return true;
    }

    void Mesh::CreateVertexAndIndexBuffers(ImportedComponent& component)
    {
        m_Name = component.Name;
        m_VertexCount = static_cast<uint32_t>(component.VertexData.size());
        m_VertexBuffer = VertexBuffer::Create(component.VertexData.data(), static_cast<uint32_t>(sizeof(PBRVertex) * component.VertexData.size()));
        m_IndexBuffer = IndexBuffer::Create(component.Indices.data(), static_cast<uint32_t>(component.Indices.size()));
    }
}