#pragma once

#include "Core/Core.h"
#include <glm/glm.hpp>
#include <memory>


namespace SmolEngine
{
	class OrthographicCamera;
	class VertexArray;

	class RendererAPI
	{
	public:

		enum class API
		{
			None = 0, OpenGL, Vulkan
		};

		virtual ~RendererAPI() {};

		virtual void Clear() = 0;

		virtual void Init() = 0;

		virtual void DisableDepth() = 0;
		
		virtual void BindTexture(uint32_t id) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;

		virtual void DrawLine(const Ref<VertexArray> vertexArray, uint32_t count = 0) = 0;

		virtual void DrawIndexed(const Ref<VertexArray> vertexArray, uint32_t count = 0) = 0;

		virtual void DrawFan(const Ref<VertexArray> vertexArray, uint32_t count = 0) = 0;

		virtual void DrawLight() = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};
}
