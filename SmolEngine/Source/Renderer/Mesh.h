#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	class GraphicsPipeline;
	struct ImportedData;

	class Mesh
	{
	public:

		static bool Create(const std::string& filePath, Ref<Mesh>& out_mesh);

	private:

		void Free();

		bool Init(ImportedData* data);

	private:

		Ref<GraphicsPipeline> m_Pipeline = nullptr;
		uint32_t m_VertexCount = 0;

	private:

		friend class Renderer;
		friend class PBRTestLayer;
	};
}