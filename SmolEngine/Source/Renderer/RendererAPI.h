#pragma once
#include "Core/Core.h"

#include "Renderer/OpenGL/OpenglRendererAPI.h"
#include "Renderer/Vulkan/VulkanRendererAPI.h"

#include <glm/glm.hpp>


namespace SmolEngine
{
	class VertexArray;

	class RendererAPI
	{
	public:

		/// 
		/// Main
		/// 
		
		void Clear();

		void Init();

		/// 
		/// Setters
		/// 

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		void SetClearColor(const glm::vec4& color);

		/// 
		/// Commands
		/// 
	
		// Draw

		void DrawLine(const Ref<VertexArray> vertexArray, uint32_t count = 0, size_t vertices = 0);

		void DrawTriangle(const Ref<VertexArray> vertexArray, uint32_t count = 0, size_t vertices = 0);

		void DrawFan(const Ref<VertexArray> vertexArray, uint32_t count = 0, size_t vertices = 0);

		void DisableDepth();

		void BindTexture(uint32_t id);

	private:

#ifdef SMOLENGINE_OPENGL_IMPL

		OpenglRendererAPI m_OpenglAPI = {};
#else

		VulkanRendererAPI m_VulkanAPI = {};

#endif // SMOLENGINE_OPENGL_IMPL

	};
}
