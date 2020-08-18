#pragma once

#include "Core/Core.h"
#include "Core/Renderer/RendererAPI.h"

namespace SmolEngine
{

	class OpenglRendererAPI: public RendererAPI
	{
	public:
		OpenglRendererAPI();
		void Clear() override;
		void Init() override;
		void SetClearColor(const glm::vec4& color) override;
		void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0) override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	};
}
