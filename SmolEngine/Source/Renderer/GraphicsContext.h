#pragma once
#include "Core/Core.h"

#ifdef  SMOLENGINE_OPENGL_IMPL
#include "Renderer/OpenGL/OpenglContext.h"
#include "Renderer/OpenGL/OpenglRendererAPI.h"
#else
#include "Renderer/Vulkan/VulkanContext.h"
#endif

#include "Framebuffer.h"

namespace SmolEngine 
{
	class GraphicsContext
	{
	public:

		GraphicsContext() = default;

		~GraphicsContext();

		/// Main
		
		void OnResize(uint32_t width, uint32_t height);

		void Init();

		void SetupWindow(GLFWwindow* window, uint32_t* width, uint32_t* height);

		void SwapBuffers();

		void BeginFrame();

		/// Getters

#ifdef  SMOLENGINE_OPENGL_IMPL
		OpenglRendererAPI* GetOpenglRendererAPI() { return m_RendererAPI; }
#else
		VulkanContext& GetVulkanContext() { return m_VulkanContext; }
#endif
		static GraphicsContext* GetSingleton() { return s_Instance; }

	private:

		inline static GraphicsContext*       s_Instance = nullptr;

#ifdef  SMOLENGINE_OPENGL_IMPL
		OpenglContext                        m_OpenglContext = {};
		OpenglRendererAPI*                   m_RendererAPI = nullptr;
#else
		VulkanContext                        m_VulkanContext = {};
#endif
	};
}
