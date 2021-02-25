#include "stdafx.h"
#include "Application.h"
#include "GLFW/glfw3.h"

#include "Core/Tools.h"
#include "Core/Input.h"
#include "Core/Window.h"

#include "ImGui/ImGuiLayer.h" 
#include "Events/ApplicationEvent.h"

#include "Renderer/GraphicsContext.h"

#include "../../../GameX/CppScriptingExamples.h"

namespace SmolEngine 
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		if (s_Instance != nullptr) 
		{
			NATIVE_ERROR("Application already instantiated");
			abort();
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

		ClientInit();

		//----------------------CLIENT-SIDE-INITIALIZATION----------------------//

		// Initializing WorldAdmin
		WorldAdmin::GetSingleton()->Init();

#ifndef SMOLENGINE_EDITOR
		// Loading a scene with index 0 and starting the game
		WorldAdmin::GetScene()->StartGame();
#endif
		timer.StopTimer();
		NATIVE_INFO("Initialized successfully");

		m_Running = true;
		// Starting Main Loop
		RunApp();
	}

	void Application::CloseApp()
	{
		NATIVE_INFO("State : Shutdown");

		m_Running = false;

		WorldAdmin::GetSingleton()->ShutDown();
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
		m_Window->ResizeContext(e.GetHeight(), e.GetWidth());

		// No need to block this event
		return false;
	}

	void Application::GetAppName(std::string& outName)
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

	const uint32_t Application::GetWindowHeight()
	{
		return m_Window->GetHeight();
	}

	const uint32_t Application::GetWindowWidth()
	{
		return m_Window->GetWidth();
	}

}
