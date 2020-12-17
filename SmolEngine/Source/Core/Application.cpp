#include "stdafx.h"
#include "Application.h"
#include "GLFW/glfw3.h"

#include "Tools.h"
#include "Input.h"
#include "Window.h"

#include "ImGui/ImGuiLayer.h" 
#include "Core/Events/ApplicationEvent.h"
#include "Core/Renderer/Renderer.h"

#include "../../../GameX/CppScriptingExamples.h"

#include "Core/Renderer/Vulkan/VulkanInstance.h"
#include "Core/Renderer/Vulkan/VulkanDevice.h"

namespace SmolEngine 
{
	struct MyStruct: public PhysicsTupleBehaviour {
		MyStruct() {}; void func(double val) { NATIVE_ERROR(val); }; int data = 0; void Update(float b) {} void Process() { NATIVE_ERROR("???"); }
	};

	Application* Application::s_Instance = nullptr;

	Application::Application()
		:m_EventHandler(0),
		m_LayerHandler(0), 
		m_Window(0),
		m_ImGuiLayer(0),
		m_Running(true),
		m_LastFrameTime(0.0f),
		m_WindowMinimized(false)
	{
		if (s_Instance != nullptr) 
		{
			NATIVE_ERROR("Application already instantiated");
			abort;
		}

		s_Instance = this;
	}

	Application::~Application()
	{
		NATIVE_INFO("State : Shutdown");

		m_Running = false;
		delete m_ImGuiLayer;
	}

	void Application::InitApp()
	{
		NATIVE_INFO("State = Startup");
		//---------------------------------------------------------------------///

		// Creating Startup Timer
		ToolTimer timer("<Startup Timer>");
		timer.StartTimer();

		// Initializing Event Dispatcher
		m_EventHandler = std::make_shared<EventHandler>();

		// Binding Callbacks
		m_EventHandler->OnEventFn = std::bind(&Application::OnEvent, this, std::placeholders::_1);

		// Initializing LayerManager and Related Classes
		m_LayerHandler = std::make_shared<LayerManager>();

		std::string appName;

#ifdef SMOLENGINE_OPENGL_IMPL

#ifdef SMOLENGINE_DEBUG

		appName = "SmolEngine Editor - Debug x64 (OpenGL)";
#else
		appName = "SmolEngine Editor - Release x64 (OpenGL)";
#endif

#else

#ifdef SMOLENGINE_DEBUG

		appName = "SmolEngine Editor - Debug x64 (Vulkan)";
#else
		appName = "SmolEngine Editor - Release x64 (Vulkan)";
#endif

#endif // SMOLENGINE_OPENGL_IMPL

		// Creating New GLFW Window

		m_Window = std::make_shared<Window>(appName, 1080, 1920, m_EventHandler);

#ifdef SMOLENGINE_EDITOR

		// Initializing Dear ImGui
		m_ImGuiLayer = new ImGuiLayer();
#endif

		// Initializing Scene Handler
		WorldAdmin::GetScene()->Init();

#ifdef SMOLENGINE_EDITOR

		// Pushing Dear ImGui Layer
		PushLayer(m_ImGuiLayer);

#endif
		// Initializing Platform Specific Renderer
		RendererCommand::Init();

		//----------------------CLIENT-SIDE-INITIALIZATION----------------------//

		ClientInit();

		//----------------------CLIENT-SIDE-INITIALIZATION----------------------//

		WorldAdmin::GetScene()->InitSystems();

#ifndef SMOLENGINE_EDITOR

		// Loading a scene with index 0 and starting the game
		WorldAdmin::GetScene()->StartGame();
#endif

		timer.StopTimer();
		NATIVE_INFO("Initialized successfully");

		// Starting Main Loop
		RunApp();
	}

	void Application::CloseApp()
	{
		NATIVE_INFO("State : Shutdown");

		m_Running = false;

		WorldAdmin::GetScene()->ShutDown();
		m_Window->ShutDown();

		exit(0);
	}

	void Application::RunApp()
	{
		NATIVE_INFO("State = Runtime");
		while (m_Running)
		{
			// Process glfw events
			m_Window->ProcessEvents();

			// Calculating DeltaTime
			float time = (float)glfwGetTime();
			DeltaTime deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (m_WindowMinimized)
			{
				continue;
			}

			// Begin New Frame
			m_Window->BeginFrame();

#ifdef SMOLENGINE_EDITOR

			// Updating ImGui
			m_ImGuiLayer->OnBegin();
			for (Layer* layer : *m_LayerHandler)
			{
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->OnEnd();
#endif

			// Updating Layers

			for (Layer* layer : *m_LayerHandler)
			{
				layer->OnUpdate(deltaTime);
			}

			m_Window->SwapBuffers();
		}
	}

	void Application::OnEvent(SmolEngine::Event& event)
	{
		// Setting Event Hooks - Functions Must Always Return Boolean: True If Event Should Be Blocked, False If Not

		//S_BIND_EVENT_CATEGORY(Application, OnWindowClose, EventCategory::S_EVENT_APPLICATION, event);

		S_BIND_EVENT_TYPE(Application, OnWindowClose, EventType::S_WINDOW_CLOSE, event);
		S_BIND_EVENT_TYPE(Application, OnWindowResize, EventType::S_WINDOW_RESIZE, event);

		// Sending Events To The Layer Stack
		for (auto result = m_LayerHandler->end(); result != m_LayerHandler->begin();)
		{
			(*--result)->OnEvent(event);
			if (event.m_Handled)
			{
				break;
			}
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerHandler->AddLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerHandler->AddOverlay(layer);
	}

	void Application::PopLayer()
	{

	}

	bool Application::OnWindowClose(Event& e)
	{
		CloseApp();

		return true;
	}

	bool Application::OnWindowResize(Event& event)
	{
		auto& e = static_cast<WindowResizeEvent&>(event);
		if (e.GetHeight() == 0 || e.GetWidth() == 0)
		{
			m_WindowMinimized = true; return false;
		}

		m_WindowMinimized = false; 

		m_Window->ResizeContext(e.GetWidth(), e.GetHeight());
		Renderer::OnWidowResize(e.GetHeight(), e.GetWidth());

		// No need to block this event
		return false;
	}

	const uint32_t Application::GetWindowHeight()
	{
		return m_Window->GetHeight();
	}

	const uint32_t Application::GetWindowWidth()
	{
		return m_Window->GetWidth();
	}

}
