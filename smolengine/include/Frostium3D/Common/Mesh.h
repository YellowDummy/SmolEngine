#pragma once
#include "Common/Core.h"
#include "Common/Common.h"

namespace Frostium
{
	struct ImportedDataGlTF;

	class VertexBuffer;
	class IndexBuffer;
	class BufferLayout;
	class Texture;
	class Material;

	class Mesh
	{
	public:

		Mesh();
		~Mesh();

		// Factory
		static void Create(const std::string& filePath, Mesh* out_mesh);

		// Find
		Mesh* FindSubMeshByIndex(uint32_t index);
		Mesh* FindSubMeshByName(const std::string& name);

		// Getters
		VertexBuffer* GetVertexBuffer() { return m_VertexBuffer; }
		IndexBuffer* GetIndexBuffer() { return m_IndexBuffer; }
		const uint32_t GetVertexCount() const { return m_VertexCount; }
		const std::vector<Mesh*>& GetSubMeshes() const { return m_SubMeshes; }
		const std::vector<Mesh*>& GetAllMeshes() const { return m_Meshes; }
		const std::string& GetName() const;

	private:

		void Free();
		void FindAllMeshes();
		bool Init(ImportedDataGlTF* data);

	private:

		bool                     m_Initialized = false;
		uint32_t                 m_VertexCount = 0;
		VertexBuffer*            m_VertexBuffer = nullptr;
		IndexBuffer*             m_IndexBuffer = nullptr;
		std::string              m_Name = "";
		std::vector<Mesh*>       m_SubMeshes;
		std::vector<Mesh*>       m_Meshes;

	private:

		friend class Renderer;
	};
}