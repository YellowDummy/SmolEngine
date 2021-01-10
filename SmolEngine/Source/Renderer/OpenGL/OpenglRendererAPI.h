#pragma once

#include "Core/Core.h"

namespace SmolEngine
{
	class OrthographicCamera;
	class VertexArray;
	
	class OpenglRendererAPI
	{
	public:

		OpenglRendererAPI();

		~OpenglRendererAPI();

		/// Main
		
		void Clear();

		void Init();

		/// Setters

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		void SetClearColor(const glm::vec4& color);

		// Draw

		void DrawTriangle (const Ref<VertexArray> vertexArray, uint32_t count = 0, size_t vertices = 0);

		void DrawLine(const Ref<VertexArray> vertexArray, uint32_t count = 0, size_t vertices = 0);

		void DrawFan(const Ref<VertexArray> vertexArray, uint32_t count = 0, size_t vertices = 0);

		void DisableDepth();

		void BindTexture(uint32_t id);
	};
}
