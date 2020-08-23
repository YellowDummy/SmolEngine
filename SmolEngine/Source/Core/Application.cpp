#include "stdafx.h"
#include "Application.h"
#include "GLFW/glfw3.h"
#include "Tools.h"
#include "Input.h"
#include "Window.h"
#include "ImGui/ImGuiLayer.h" 
#include "Core/Events/ApplicationEvent.h"
#include "Core/Renderer/Renderer.h"

namespace SmolEngine 
{

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
		NATIVE_INFO("Initializing a framework...");
		NATIVE_INFO("State = Startup");
		//---------------------------------------------------------------------///

		//Creating Timer
		ToolTimer timer("<Startup Timer>");
		timer.StartTimer();

		//Initializing Event & EventHandler Classes
		m_EventHandler = std::make_shared<EventHandler>();

		//Binding Callbacks
		m_EventHandler->OnEventFn = std::bind(&Application::OnEvent, this, std::placeholders::_1);

		//Initializing Layer & LayerManager
		m_LayerHandler = std::make_shared<LayerManager>();

		//Initializing Main Window
		m_Window = std::make_shared<Window>(std::string("SmolEngine Editor - v0.1"), 1080, 1920, m_EventHandler);

		//Initializing GUI
		m_ImGuiLayer = new ImGuiLayer();
		//---------------------------------------------------------------------///
		NATIVE_INFO("Initialized successfully");
		timer.StopTimer();

		//----------------------CLIENT-SIDE-INITIALIZATION----------------------//

		ClientInit();

		//----------------------CLIENT-SIDE-INITIALIZATION----------------------//

		//Creating & Pushing ImGui Layer As Overlay
		PushOverlay(m_ImGuiLayer);

		//Initializing Platform Specific Renderer
		RendererCommand::Init();

		//Starting Main Loop
		RunApp();
	}

	void Application::CloseApp()
	{
		NATIVE_INFO("State : Shutdown");
		m_Window->ShutDown();
		m_Running = false;
		exit(0);
	}

	void Application::RunApp()
	{
		NATIVE_INFO("State = Runtime");
		while (m_Running)
		{
			//Calculating DeltaTime
			float time = (float)glfwGetTime();
			DeltaTime deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			//Updating Layer Logic
			if (!m_WindowMinimized)
			{
				for (Layer* layer : *m_LayerHandler)
				{
					layer->OnUpdate(deltaTime);
				}
			}

			//--------------------IMGUI-RENDERER-----------------//
			m_ImGuiLayer->OnBegin();
			for (Layer* layer : *m_LayerHandler)
			{
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->OnEnd();
			//--------------------IMGUI-RENDERER-----------------//

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(SmolEngine::Event& event)
	{
		//Setting Event Hooks - Functions Must Always Return Boolean: True If Event Should Be Blocked, False If Not

		//S_BIND_EVENT_CATEGORY(Application, OnWindowClose, EventCategory::S_EVENT_APPLICATION, event);
		S_BIND_EVENT_TYPE(Application, OnWindowClose, EventType::S_WINDOW_CLOSE, event);
		S_BIND_EVENT_TYPE(Application, OnWindowResize, EventType::S_WINDOW_RESIZE, event);

		//Sending Events To The Layer Stack
		for (auto result = m_LayerHandler->end(); result != m_LayerHandler->begin();)
		{
			(*--result)->OnEvent(event);
			if (event.m_Handled)
			{
				break;
			}
		}
	}

	void Application::BindAction(const int eventType, const int key, const void *event_ptr)
	{
		m_EventHandler->BindAction(eventType, key, event_ptr);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerHandler->AddLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerHandler->AddOverlay(layer);
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
		Renderer::OnWidowResize(e.GetHeight(), e.GetWidth());
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
