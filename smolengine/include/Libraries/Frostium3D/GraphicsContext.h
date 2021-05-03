#pragma once
#include "EditorCamera.h"

#ifdef  FROSTIUM_OPENGL_IMPL
#include "OpenGL/OpenglContext.h"
#include "OpenGL/OpenglRendererAPI.h"
#else
#include "Vulkan/VulkanContext.h"
#endif

#include "Common/RendererShared.h"
#include "Common/Core.h"
#include "Common/Window.h"
#include "Common/Framebuffer.h"
#include "Common/Events.h"
#include "Common/Texture.h"
#include "Common/Flags.h"
#include "Common/Camera.h"

#include "Utils/Frustum.h"
#include "ImGUI/ImGuiContext.h"

#include <functional>

namespace Frostium 
{
	struct WindowCreateInfo;
	struct WindowData;
	struct GraphicsContextState;
	struct Renderer2DStorage;
	struct RendererStorage;
	class Framebuffer;
	class MaterialLibrary;

	struct GraphicsContextInitInfo
	{
		bool                     bTargetsSwapchain = true;
		Flags                    Flags = Features_Renderer_3D_Flags | Features_Renderer_2D_Flags | Features_ImGui_Flags;
		MSAASamples              eMSAASamples = MSAASamples::SAMPLE_COUNT_MAX_SUPPORTED;
		ShadowMapSize            eShadowMapSize = ShadowMapSize::SIZE_8;
		Camera*                  pDefaultCamera = nullptr;
		WindowCreateInfo*        pWindowCI = nullptr;
		Renderer2DStorage*       pRenderer2DStorage = nullptr;
		RendererStorage*         pRendererStorage = nullptr;
		std::string              ResourcesFolderPath = "../resources/";
	};

	class GraphicsContext
	{
	public:
		GraphicsContext() = default;
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
	    Camera* GetDefaultCamera();
		GLFWwindow* GetNativeWindow();
		Window* GetWindow();
	    WindowData* GetWindowData();
		Frustum* GetFrustum();
		float GetGltfTime() const;
		float GetDeltaTime() const;
		float GetLastFrameTime() const;
#ifdef  FROSTIUM_OPENGL_IMPL
		static OpenglRendererAPI* GetOpenglRendererAPI();
#else
		static VulkanContext& GetVulkanContext();
#endif
		// Helpers
		DeltaTime CalculateDeltaTime();
		bool IsWindowMinimized() const;
		// Events
		void OnResize(uint32_t* width, uint32_t* height);
	private:
		void OnEvent(Event& event);
		bool InitRenderer2DStorage(Renderer2DStorage* storage);
		bool InitRendererStorage(RendererStorage* storage, ShadowMapSize shadow_map_size);
	private:
		static GraphicsContext*         s_Instance;
		MSAASamples                     m_MSAASamples;
		Flags                           m_Flags;
		Texture*                        m_DummyTexure = nullptr;
		Camera*                         m_DefaultCamera = nullptr;
		MaterialLibrary*                m_MaterialLibrary = nullptr;
		GraphicsContextState*           m_State = nullptr;
		Renderer2DStorage*              m_Renderer2DStorage = nullptr;
		RendererStorage*                m_RendererStorage = nullptr;
		float                           m_LastFrameTime = 1.0f;
		float                           m_DeltaTime = 0.0f;
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
