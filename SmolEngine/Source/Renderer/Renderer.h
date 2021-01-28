#pragma once
#include "Core/Core.h"
#include "Renderer/Renderer2D.h"
#include "Core/AssetManager.h"
#include <glm/glm.hpp>
#include <memory>

namespace SmolEngine
{
	class Shader;

	class Renderer
	{
	public:

		static void Init();

	private:
		struct SceneData 
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};
}
