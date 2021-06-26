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
#include "Common/Mesh.h"
#include "Common/CubeMap.h"
#include "Common/Framebuffer.h"
#include "Common/Events.h"
#include "Common/Texture.h"
#include "Common/Flags.h"
#include "Common/Camera.h"

#include "Utils/Frustum.h"
#include "ImGUI/ImGuiContext.h"

#include <functional>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	struct WindowCreateInfo;
	struct WindowData;
	struct GraphicsContextState;
	struct Renderer2DStorage;
	struct RendererStorage;
	class CubeMap;
	class Framebuffer;
	class MaterialLibrary;
	class JobsSystemInstance;

	struct GraphicsContextInitInfo
	{
		bool                          bTargetsSwapchain = true;
		bool                          bAutoResize = true;
		Flags                         Flags = Features_Renderer_3D_Flags | Features_Renderer_2D_Flags | Features_ImGui_Flags;
		MSAASamples                   eMSAASamples = MSAASamples::SAMPLE_COUNT_1;
		ShadowMapSize                 eShadowMapSize = ShadowMapSize::SIZE_8;
		Camera*                       pDefaultCamera = nullptr;
		WindowCreateInfo*             pWindowCI = nullptr;
		Renderer2DStorage*            pRenderer2DStorage = nullptr;
		RendererStorage*              pRendererStorage = nullptr;
		std::string                   ResourcesFolderPath = "../resources/";
	};

	class GraphicsContext
	{
	public:

		GraphicsContext() = default;
		GraphicsContext(GraphicsContextInitInfo* info);
		~GraphicsContext();
								      
		void                          ProcessEvents();
		void                          BeginFrame(DeltaTime time);
		void                          UpdateSceneInfo(BeginSceneInfo* sceneInfo = nullptr);
		void                          SwapBuffers();
		void                          ShutDown();
								      
		// Getters				      
		static GraphicsContext*       GetSingleton();
		Camera*                       GetDefaultCamera() const;
		Framebuffer*                  GetFramebuffer() const;
		GLFWwindow*                   GetNativeWindow();
		Window*                       GetWindow() const;
	    WindowData*                   GetWindowData();
		Frustum*                      GetFrustum() const;
		Mesh*                         GetBoxMesh() const;
		Mesh*                         GetCapsuleMesh() const;
		Mesh*                         GetSphereMesh() const;
		Texture*                      GetWhiteTexture() const;
		float                         GetGltfTime() const;
		float                         GetDeltaTime() const;
		float                         GetLastFrameTime() const;
		std::string                   GetResourcesPath() const;
#ifdef  FROSTIUM_OPENGL_IMPL	      
		static OpenglRendererAPI*     GetOpenglRendererAPI();
#else							      
		static VulkanContext&         GetVulkanContext();
#endif
#ifdef FROSTIUM_SMOLENGINE_IMPL
		JobsSystemInstance*            GetJobsSystemInstance();
#endif
		// Setters				      
		void                          SetEventCallback(std::function<void(Event&)> callback);
		void                          SetFramebufferSize(uint32_t width, uint32_t height);

		DeltaTime                     CalculateDeltaTime();
		bool                          IsWindowMinimized() const;      
		void                          Resize(uint32_t* width, uint32_t* height);

	private:					      
		void                          OnEvent(Event& event);
		bool                          LoadMeshes();
		bool                          InitRenderer2DStorage(Renderer2DStorage* storage);
		bool                          InitRendererStorage(RendererStorage* storage, ShadowMapSize shadow_map_size);

	private:	

		static GraphicsContext*       s_Instance;
		Texture*                      m_DummyTexure = nullptr;
		CubeMap*                      m_DummyCubeMap = nullptr;
		Camera*                       m_DefaultCamera = nullptr;
		MaterialLibrary*              m_MaterialLibrary = nullptr;
		GraphicsContextState*         m_State = nullptr;
		Renderer2DStorage*            m_Renderer2DStorage = nullptr;
		RendererStorage*              m_RendererStorage = nullptr;
		Frustum*                      m_Frustum = nullptr;
		Framebuffer*                  m_Framebuffer = nullptr;
		Window*                       m_Window = nullptr;
		ImGuiContext*                 m_ImGuiContext = nullptr;
		Mesh*                         m_BoxMesh = nullptr;
		Mesh*                         m_SphereMesh = nullptr;
		Mesh*                         m_CapsuleMesh = nullptr;

#ifdef FROSTIUM_SMOLENGINE_IMPL
		JobsSystemInstance*           m_JobsSystem = nullptr;
#endif
		MSAASamples                   m_MSAASamples = MSAASamples::SAMPLE_COUNT_MAX_SUPPORTED;
		Flags                         m_Flags = Features_Renderer_3D_Flags | Features_Renderer_2D_Flags;
		float                         m_LastFrameTime = 1.0f;
		float                         m_DeltaTime = 0.0f;
#ifdef  FROSTIUM_OPENGL_IMPL		  
		OpenglContext                 m_OpenglContext = {};
		OpenglRendererAPI* m_RendererAPI = nullptr;
#else								  
		VulkanContext                 m_VulkanContext = {};
#endif
		EventSender                   m_EventHandler = {};
		SceneData                     m_SceneData = {};
		std::string                   m_ResourcesFolderPath = "";
		std::function<void(Event&)>   m_EventCallback;

	private:

		friend class GraphicsPipeline;
		friend class DeferredRenderer;
		friend class Renderer2D;
		friend class DebugRenderer;
		friend class ImGuiContext;
		friend class VulkanPBR;
		friend class VulkanDescriptor;
		friend class Window;
		friend class EnvironmentMap;
	};
}
