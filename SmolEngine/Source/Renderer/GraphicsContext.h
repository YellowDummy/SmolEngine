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
	class Framebuffer;

	struct GraphicsContextInitInfo
	{
		bool   bMSAA = true;
		bool   bTargetsSwapchain = true;
	};

	class GraphicsContext
	{
	public:

		GraphicsContext() = default;

		~GraphicsContext();

		/// Init

	    static bool Init(GraphicsContextInitInfo& info);

	private:
		
		// Helpers

		void OnResize(uint32_t width, uint32_t height);

		void SetupWindow(GLFWwindow* window, uint32_t* width, uint32_t* height);

		void SwapBuffers();

		void BeginFrame();

		/// Getters

#ifdef  SMOLENGINE_OPENGL_IMPL
		OpenglRendererAPI* GetOpenglRendererAPI() { return m_RendererAPI; }
#else
		VulkanContext& GetVulkanContext() { return m_VulkanContext; }
#endif
		Ref<Framebuffer> GetFramebuffer() { return m_Framebuffer; }

		static GraphicsContext* GetSingleton() { return s_Instance; }

	private:

		inline static GraphicsContext*       s_Instance = nullptr;
		Ref<Framebuffer>                     m_Framebuffer = nullptr;
		bool                                 m_Initialized = false;

#ifdef  SMOLENGINE_OPENGL_IMPL
		OpenglContext                        m_OpenglContext = {};
		OpenglRendererAPI*                   m_RendererAPI = nullptr;
#else
		VulkanContext                        m_VulkanContext = {};
#endif
		friend class Application;
		friend class GraphicsPipeline;
		friend class WorldAdmin;
		friend class EditorLayer;
		friend class Renderer;
		friend class Renderer2D;
		friend class Window;
	};
}
