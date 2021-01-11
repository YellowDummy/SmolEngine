#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	class GraphicsPipeline;

	class Mesh
	{
	public:

		static bool Create(const std::string& filePath, Ref<Mesh>& out_mesh);


		Ref<GraphicsPipeline> m_Pipeline = nullptr;

		uint32_t m_VertexCount = 0;

	private:

		friend class Renderer;
	};
}