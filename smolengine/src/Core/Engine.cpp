#include "stdafx.h"
#include "Core/Engine.h"

#include "ECS/Components/Singletons/GraphicsEngineSComponent.h"
#include "ECS/Components/Singletons/JobsSystemStateSComponent.h"

#include <Frostium3D/Extensions/JobsSystem.h>

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

		Frostium::WindowCreateInfo windowCI = {};
		windowCI.bFullscreen = false;
		windowCI.bVSync = false;
		windowCI.Width = 720;
		windowCI.Height = 480;
		windowCI.Title = "SmolEngine";

		Frostium::GraphicsContextInitInfo graphicsContextCI = {};
		graphicsContextCI.eMSAASamples = Frostium::MSAASamples::SAMPLE_COUNT_MAX_SUPPORTED;
		graphicsContextCI.eShadowMapSize = Frostium::ShadowMapSize::SIZE_8;
		graphicsContextCI.pWindowCI = &windowCI;
		graphicsContextCI.ResourcesFolderPath = "../resources/";
		graphicsContextCI.Flags = Frostium::Features_Renderer_3D_Flags | Frostium::Features_Renderer_2D_Flags
			| Frostium::Features_ImGui_Flags;

		Physics2DContextCreateInfo physicsContextCI = {};

		// User Side
		// Graphics Context
		SetGraphicsContext(&graphicsContextCI);
		GraphicsEngineSComponent* graphicsEngine = GraphicsEngineSComponent::Get();
		graphicsContextCI.pRendererStorage = &graphicsEngine->Strorage;
		graphicsContextCI.pRenderer2DStorage = &graphicsEngine->Storage2D;
		m_GraphicsContext = new Frostium::GraphicsContext(&graphicsContextCI);
		// Graphics engine and game engine use the same jobs system,
		// but different queues in use
		JobsSystemStateSComponent::GetSingleton()->Executor = m_GraphicsContext->GetJobsSystemInstance()->GetExecutor();

		// 2D Physics
		SetPhysics2DContext(&physicsContextCI);
		// Layers
		SetLayers(m_LayerHandler);
		// Scripts
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
			exit(0);
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
					layer->OnBeginFrame(deltaTime);

				m_World->OnBeginFrame();
				{
					for (Layer* layer : *m_LayerHandler)
					{
						layer->OnUpdate(deltaTime);
						layer->OnImGuiRender();
					}

					m_World->OnUpdate(deltaTime);
				}
				m_World->OnEndFrame();

				for (Layer* layer : *m_LayerHandler)
					layer->OnEndFrame(deltaTime);
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
				break;
		}
	}

	void Engine::OnWindowClose(Frostium::Event& e)
	{
		Shutdown();
	}

	void Engine::SetOnSceneLoadedCallback(const std::function<void(Scene*)>& callback)
	{
	}

	void Engine::SetOnSceneUnLoadedCallback(const std::function<void(Scene*)>& callback)
	{
	}

	Frostium::GraphicsContext* Engine::GetGraphicsContext() const
	{
		return m_GraphicsContext;
	}

	const uint32_t Engine::GetWindowHeight() const
	{
		return m_GraphicsContext->GetWindowData()->Height;
	}

	const uint32_t Engine::GetWindowWidth() const
	{
		return m_GraphicsContext->GetWindowData()->Width;
	}

}
