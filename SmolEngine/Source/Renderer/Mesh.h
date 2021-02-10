#pragma once
#include "Core/Core.h"
#include "Renderer/Shared.h"

namespace SmolEngine
{
	struct ImportedData;
	struct ImportedComponent;

	class VertexBuffer;
	class IndexBuffer;
	class BufferLayout;
	class Texture;
	class Material;

	struct MeshOffset
	{
		size_t                         VBOffset = 0;
		size_t                         IBOffset = 0;
	};

	class Mesh
	{
	public:

		static Ref<Mesh> Create(const std::string& filePath);

		// Getters

		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }

		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }

		const int32_t GetMaterialID() const { return m_MaterialID; }

		const uint32_t GetVertexCount() const { return m_VertexCount; }

		std::vector<Ref<Mesh>>& GetSubMeshes() { return m_SubMeshes; }

		// Setters

		void SetMaterialID(int32_t ID) { m_MaterialID = ID; }

	private:

		void Free();

		bool Init(ImportedData* data);

		void CreateVertexAndIndexBuffers(ImportedComponent& component);

	private:

		uint32_t                           m_VertexCount = 0;
		int32_t                            m_MaterialID = 0; // default material

		Ref<VertexBuffer>                  m_VertexBuffer = nullptr;
		Ref<IndexBuffer>                   m_IndexBuffer = nullptr;

		std::vector<Ref<Mesh>>             m_SubMeshes;

	private:

		friend class Renderer;
	};
}