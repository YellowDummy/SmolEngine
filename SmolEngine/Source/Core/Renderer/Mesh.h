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

	private:

		Ref<GraphicsPipeline> m_Pipeline = nullptr;

	private:

		friend class Renderer;
	};
}