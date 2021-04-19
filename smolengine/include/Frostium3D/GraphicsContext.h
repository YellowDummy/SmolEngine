#pragma once
#include "EditorCamera.h"

#ifdef  FROSTIUM_OPENGL_IMPL
#include "OpenGL/OpenglContext.h"
#include "OpenGL/OpenglRendererAPI.h"
#else
#include "Vulkan/VulkanContext.h"
#endif

#include "Common/Core.h"
#include "Common/Window.h"
#include "Common/Framebuffer.h"
#include "Common/Events.h"
#include "Common/Texture.h"
#include "Common/Flags.h"

#include "Utils/Frustum.h"
#include "ImGUI/ImGuiContext.h"

#include <functional>

namespace Frostium 
{
	struct WindowCreateInfo;
	struct WindowData;
	class Framebuffer;
	class MaterialLibrary;

	enum class ShadowMapSize : uint16_t
	{
		SIZE_2,
		SIZE_4,
		SIZE_8,
		SIZE_16
	};

	struct GraphicsContextInitInfo
	{
		bool                     bTargetsSwapchain = true;
		Flags                    Flags = Features_Renderer_3D_Flags | Features_HDR_Flags;
		MSAASamples              eMSAASamples = MSAASamples::SAMPLE_COUNT_MAX_SUPPORTED;
		ShadowMapSize            eShadowMapSize = ShadowMapSize::SIZE_8;
		EditorCameraCreateInfo*  pEditorCameraCI = nullptr;
		WindowCreateInfo*        pWindowCI = nullptr;
		std::string              ResourcesFolderPath = "../resources/";
	};

	class GraphicsContext
	{
	public:

		GraphicsContext(GraphicsContextInitInfo* info);
		~GraphicsContext();

		void ProcessEvents();
		void BeginFrame(DeltaTime time);
		void SwapBuffers();
		void ShutDown();

		// Setters
		void SetEventCallback(std::function<void(Event&)> callback);

		// Getters
		static GraphicsContext* GetSingleton();
		Framebuffer* GetFramebuffer();
	    EditorCamera* GetEditorCamera();
		GLFWwindow* GetNativeWindow();
		Window* GetWindow();
	    WindowData* GetWindowData();
		Frustum* GetFrustum();

		float GetTime() const;
		float GetLastFrameTime() const;

		// Helpers
		DeltaTime CalculateDeltaTime();
		bool IsWindowMinimized() const;

#ifdef  FROSTIUM_OPENGL_IMPL
		static OpenglRendererAPI* GetOpenglRendererAPI();
#else
		static VulkanContext& GetVulkanContext();
#endif

	private:

		// Events
		void OnResize(uint32_t* width, uint32_t* height);
		void OnEvent(Event& event);

	private:

		static GraphicsContext*         s_Instance;
		MSAASamples                     m_MSAASamples;
		Flags                           m_Flags;
		Texture*                        m_DummyTexure = nullptr;
		EditorCamera*                   m_EditorCamera = nullptr;
		MaterialLibrary*                m_MaterialLibrary = nullptr;
		bool                            m_Initialized = false;
		bool                            m_WindowMinimized = false;
		const bool                      m_UseImGUI = false;
		const bool                      m_UseEditorCamera = false;
		float                           m_LastFrameTime = 1.0f;
#ifdef  FROSTIUM_OPENGL_IMPL
		OpenglContext                   m_OpenglContext = {};
		OpenglRendererAPI*              m_RendererAPI = nullptr;
#else
		VulkanContext                   m_VulkanContext = {};
#endif
		Frustum                         m_Frustum = {};
		Framebuffer                     m_Framebuffer = {};
		Window                          m_Window = {};
		ImGuiContext                    m_ImGuiContext = {};
		EventSender                     m_EventHandler = {};

		std::string                     m_ResourcesFolderPath = "";
		std::function<void(Event&)>     m_EventCallback;

	private:

		friend class GraphicsPipeline;
		friend class Renderer;
		friend class Renderer2D;
		friend class ImGuiContext;
		friend class VulkanPBR;
		friend class VulkanDescriptor;
		friend class Window;
	};
}
