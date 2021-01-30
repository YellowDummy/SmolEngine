#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	struct ImportedData;

	class VertexBuffer;
	class IndexBuffer;
	class BufferLayout;
	class Texture;

	class Mesh
	{
	public:

		static Ref<Mesh> Create(const std::string& filePath);

	private:

		void Free();

		bool Init(ImportedData* data);

	private:

		Ref<VertexBuffer> m_VertexBuffer = nullptr;
		Ref<IndexBuffer> m_IndexBuffer = nullptr;

		uint32_t m_VertexCount = 0;

		std::vector<Ref<Texture>> m_Textures;

	private:

		friend class Renderer;
		friend class PBRTestLayer;
	};
}