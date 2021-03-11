#include "stdafx.h"
#include "Engine.h"
#include "GLFW/glfw3.h"

#include "Core/Tools.h"
#include "Core/Input.h"
#include "Core/Window.h"

#include "ImGui/ImGuiLayer.h" 
#include "Events/ApplicationEvent.h"

#include "Renderer/GraphicsContext.h"
#include "ECS/Systems/JobsSystem.h"

namespace SmolEngine 
{
	Engine* Engine::s_Instance = nullptr;

	Engine::Engine()
	{
		if (s_Instance != nullptr) 
		{
			NATIVE_ERROR("Engine already instantiated");
			abort();
		}

		s_Instance = this;
	}

	Engine::~Engine()
	{
		NATIVE_INFO("State : Shutdown");

		m_Running = false;
		delete m_ImGuiLayer;
	}

	struct SomeData
	{
		float x = 0.0f;
		int b = 100;
		std::string name = "NoName";
	};

	void Engine::Init()
	{
		NATIVE_INFO("State = Startup");
		//---------------------------------------------------------------------///

		// Creating Startup Timer
		ToolTimer timer("<Startup Timer>");
		timer.StartTimer();

		// Initializing World Admin
		m_World = new WorldAdmin();
		m_World->Init();

		// Initializing Event Dispatcher
		m_EventHandler = std::make_shared<EventHandler>();

		// Binding Callbacks
		m_EventHandler->OnEventFn = std::bind(&Engine::OnEvent, this, std::placeholders::_1);

		// Initializing LayerManager and Related Classes
		m_LayerHandler = std::make_shared<LayerManager>();

		std::string appName = "";
		GetAppName(appName);

		WindowCreateInfo windowCI = {};
		{
			windowCI.Height = 1080;
			windowCI.Width = 1920;
			windowCI.Title = appName;
			windowCI.EventHandler = m_EventHandler;
		}

		// Creating New GLFW Window
		m_Window = std::make_shared<Window>(windowCI);

#ifdef SMOLENGINE_EDITOR
		// Initializing Dear ImGui
		m_ImGuiLayer = new ImGuiLayer();
#endif

#ifdef SMOLENGINE_EDITOR
		// Pushing Dear ImGui Layer
		PushLayer(m_ImGuiLayer);
#endif

		//----------------------CLIENT-SIDE-INITIALIZATION----------------------//

		OnEngineInitialized();

		//----------------------CLIENT-SIDE-INITIALIZATION----------------------//

#ifndef SMOLENGINE_EDITOR
		// Loading a scene with index 0 and starting the game
		WorldAdmin::GetScene()->StartGame();
#endif
		timer.StopTimer();
		NATIVE_INFO("Initialized successfully");
		m_Running = true;
		// Starting Main Loop
		Run();
	}

	void Engine::Shutdown()
	{
		NATIVE_INFO("State : Shutdown");

		m_Running = false;

		WorldAdmin::GetSingleton()->ShutDown();
		m_Window->ShutDown();
		exit(0);
	}

	void Engine::Run()
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
				continue;

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

	void Engine::OnEvent(Event& event)
	{
		if (event.m_EventType == (uint32_t)EventType::S_WINDOW_CLOSE)
			OnWindowClose(event);

		if (event.m_EventType == (uint32_t)EventType::S_WINDOW_RESIZE)
			OnWindowResize(event);

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

	void Engine::PushLayer(Layer* layer)
	{
		m_LayerHandler->AddLayer(layer);
	}

	void Engine::PushOverlay(Layer* layer)
	{
		m_LayerHandler->AddOverlay(layer);
	}

	void Engine::PopLayer()
	{

	}

	void Engine::OnWindowClose(Event& e)
	{
		Shutdown();
	}

	void Engine::OnWindowResize(Event& event)
	{
		auto& e = static_cast<WindowResizeEvent&>(event);
		if (e.GetHeight() == 0 || e.GetWidth() == 0)
		{
			m_WindowMinimized = true; 
			return;
		}

		m_WindowMinimized = false; 
		m_Window->ResizeContext(e.GetHeight(), e.GetWidth());
	}

	void Engine::GetAppName(std::string& outName)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
#ifdef SMOLENGINE_DEBUG
		outName = "SmolEngine Editor - Debug x64 (OpenGL)";
#else
		outName = "SmolEngine Editor - Release x64 (OpenGL)";
#endif
#else
#ifdef SMOLENGINE_DEBUG
		outName = "SmolEngine Editor - Debug x64 (Vulkan)";
#else
		outName = "SmolEngine Editor - Release x64 (Vulkan)";
#endif
#endif 
	}

	const uint32_t Engine::GetWindowHeight()
	{
		return m_Window->GetHeight();
	}

	const uint32_t Engine::GetWindowWidth()
	{
		return m_Window->GetWidth();
	}

}
