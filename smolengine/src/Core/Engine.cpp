#include "stdafx.h"
#include "Core/Engine.h"

namespace SmolEngine 
{
	Engine* Engine::s_Instance = nullptr;

	Engine::Engine()
	{
		if (s_Instance != nullptr) 
			std::runtime_error("Engine already instantiated");

		s_Instance = this;
	}

	Engine::~Engine()
	{
		NATIVE_INFO("State : Shutdown");

		m_Running = false;
	}

	void Engine::Init()
	{
		NATIVE_INFO("State = Startup");
		//---------------------------------------------------------------------///
		m_LayerHandler = new LayerManager();
		m_ScriptingSystem = new ScriptingSystem();
		m_World = new WorldAdmin();
		m_World->Init();

		bool bSwapChain = true;
		Frostium::WindowCreateInfo windowCI = {};
		windowCI.bFullscreen = false;
		windowCI.bVSync = false;
		windowCI.Width = 720;
		windowCI.Height = 480;
		windowCI.Title = "SmolEngine";

		Frostium::EditorCameraCreateInfo editorCameraCI = {};
		Frostium::GraphicsContextInitInfo graphicsContextCI = {};
		graphicsContextCI.bTargetsSwapchain = bSwapChain;
		graphicsContextCI.eMSAASamples = Frostium::MSAASamples::SAMPLE_COUNT_MAX_SUPPORTED;
		graphicsContextCI.eShadowMapSize = Frostium::ShadowMapSize::SIZE_8;
		graphicsContextCI.pEditorCameraCI = &editorCameraCI;
		graphicsContextCI.pWindowCI = &windowCI;
		graphicsContextCI.ResourcesFolderPath = "../resources/";
		graphicsContextCI.Flags = Frostium::Features_Renderer_3D_Flags | Frostium::Features_Renderer_2D_Flags
			| Frostium::Features_ImGui_Flags | Frostium::Features_HDR_Flags;

		PhysicsContextCreateInfo physicsContextCI = {};

		// User Side
		SetGraphicsContext(&graphicsContextCI);
		m_GraphicsContext = new Frostium::GraphicsContext(&graphicsContextCI);
		SetPhysicsContext(&physicsContextCI);
		SetLayers(m_LayerHandler);
		SetScripts(m_ScriptingSystem);

		m_GraphicsContext->SetEventCallback(std::bind(&Engine::OnEvent, this, std::placeholders::_1));
		NATIVE_INFO("Initialized successfully");
		m_Running = true;
		Run();
	}

	void Engine::Shutdown()
	{
		if (m_Running)
		{
			NATIVE_INFO("State : Shutdown");

			m_Running = false;
			WorldAdmin::GetSingleton()->ShutDown();
			m_GraphicsContext->ShutDown();

			delete m_World, m_LayerHandler, m_GraphicsContext, m_ScriptingSystem;

		}
	}

	void Engine::Run()
	{
		NATIVE_INFO("State = Runtime");
		while (m_Running)
		{
			m_GraphicsContext->ProcessEvents();
			Frostium::DeltaTime deltaTime = m_GraphicsContext->CalculateDeltaTime();

			if (m_GraphicsContext->IsWindowMinimized())
				continue;

			m_GraphicsContext->BeginFrame(deltaTime);
			{
				for (Layer* layer : *m_LayerHandler)
					layer->OnUpdate(deltaTime);
			}
			m_GraphicsContext->SwapBuffers();

		}
	}

	void Engine::OnEvent(Frostium::Event& e)
	{
		if (e.IsType(Frostium::EventType::WINDOW_CLOSE))
			OnWindowClose(e);

		for (auto result = m_LayerHandler->end(); result != m_LayerHandler->begin();)
		{
			(*--result)->OnEvent(e);
			if (e.m_Handled)
			{
				break;
			}
		}
	}

	void Engine::OnWindowClose(Frostium::Event& e)
	{
		Shutdown();
		exit(0);
	}

	const uint32_t Engine::GetWindowHeight()
	{
		return m_GraphicsContext->GetWindowData()->Height;
	}

	const uint32_t Engine::GetWindowWidth()
	{
		return m_GraphicsContext->GetWindowData()->Width;
	}

}
