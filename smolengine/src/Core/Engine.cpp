#include "stdafx.h"
#include "Core/Engine.h"

#include "ECS/Systems/ScriptingSystem.h"
#include "ECS/Components/Singletons/GraphicsEngineSComponent.h"
#include "ECS/Components/Singletons/JobsSystemStateSComponent.h"

#include <Frostium3D/Extensions/JobsSystemInstance.h>

namespace SmolEngine 
{
	Engine* Engine::s_Instance = nullptr;

	Engine::Engine()
	{
		if (s_Instance != nullptr)
		{
			RUNTIME_ERROR("Engine already instantiated.");
		}

		s_Instance = this;
		SLog* log = new SLog("SmoleEngine");
	}

	Engine::~Engine()
	{
		NATIVE_INFO("[Engine]: State = Shutdown");
		m_Running = false;
	}

	void Engine::Init()
	{
		NATIVE_INFO("[Engine]: State = Startup");
		//---------------------------------------------------------------------///

		m_LayerHandler = new LayerManager();
		m_World = new WorldAdmin();

		WindowCreateInfo windowCI = {};
		windowCI.bFullscreen = false;
		windowCI.bVSync = false;
		windowCI.Width = 720;
		windowCI.Height = 480;
		windowCI.Title = "SmolEngine";

		GraphicsContextInitInfo graphicsContextCI = {};
		graphicsContextCI.eMSAASamples = MSAASamples::SAMPLE_COUNT_1;
		graphicsContextCI.eShadowMapSize = ShadowMapSize::SIZE_8;
		graphicsContextCI.pWindowCI = &windowCI;
		graphicsContextCI.ResourcesFolderPath = "../resources/";
		graphicsContextCI.Flags = Features_Renderer_3D_Flags | Features_Renderer_2D_Flags | Features_ImGui_Flags;


		// User Side
		// Graphics Context
		OnGraphicsModuleCreation(&graphicsContextCI);
		GraphicsEngineSComponent* graphicsEngine = GraphicsEngineSComponent::Get();
		graphicsContextCI.pRendererStorage = &graphicsEngine->Strorage;
		graphicsContextCI.pRenderer2DStorage = &graphicsEngine->Storage2D;
		m_GraphicsContext = new GraphicsContext(&graphicsContextCI);
		m_GraphicsContext->SetEventCallback(std::bind_front(&Engine::OnEvent, this));
		// Graphics engine and game engine use the same jobs system, but different queues in use
		JobsSystemStateSComponent::GetSingleton()->Executor = m_GraphicsContext->GetJobsSystemInstance()->GetExecutor();

		EngineModuleCreateInfo engineContextCI = {};
		OnEngineModuleCreation(&engineContextCI);
		m_AssetsFolder = engineContextCI.AssetsFolder;

		PhysicsModuleCreateInfo physicsContextCI = {};
		OnPhysicsModuleCreation(&physicsContextCI);

		OnLayerModuleCreation(m_LayerHandler);
		{
			Scope<ScriptingSystem> system = std::make_unique<ScriptingSystem>();
			OnScriptModuleCreation(system.get());
		}
		OnWorldAdminModuleCreation(m_World->m_State);
		OnInitializationComplete(m_World);

		NATIVE_INFO("[Engine]: Initialized successfully");
		m_Running = true;
		Run();
	}

	void Engine::Shutdown()
	{
		if (m_Running)
		{
			NATIVE_INFO("[Engine]: State = Shutdown");

			m_Running = false;
			m_GraphicsContext->ShutDown();
			exit(0);
		}
	}

	void Engine::Run()
	{
		NATIVE_INFO("[Engine]: State = Runtime");
		uint32_t frames = 0;
		float t1 = m_GraphicsContext->GetGltfTime();

		while (m_Running)
		{
			m_GraphicsContext->ProcessEvents();

			DeltaTime deltaTime = m_GraphicsContext->CalculateDeltaTime();
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

			// Calculates FPS
			{
				float t2 = m_GraphicsContext->GetGltfTime();
				if ((t2 - t1) > 1.0 || frames == 0)
				{
					m_FPSCount = static_cast<float>(frames) / (t2 - t1);
					t1 = t2;
					frames = 0;
				}
				frames++;
			}
		}
	}

	void Engine::OnEvent(Event& e)
	{
		if (e.IsType(EventType::WINDOW_CLOSE))
			OnWindowClose(e);

		for (auto result = m_LayerHandler->end(); result != m_LayerHandler->begin();)
		{
			(*--result)->OnEvent(e);
			if (e.m_Handled)
				break;
		}
	}

	void Engine::OnWindowClose(Event& e)
	{
		Shutdown();
	}

	void Engine::SetOnSceneLoadedCallback(const std::function<void(Scene*)>& callback)
	{
	}

	void Engine::SetOnSceneUnLoadedCallback(const std::function<void(Scene*)>& callback)
	{
	}

	GraphicsContext* Engine::GetGraphicsContext() const
	{
		return m_GraphicsContext;
	}

	uint32_t Engine::GetWindowHeight() const
	{
		return m_GraphicsContext->GetWindowData()->Height;
	}

	float Engine::GetFPSCount() const
	{
		return m_FPSCount;
	}

	glm::ivec2 Engine::GetViewPortSize() const
	{
		const auto& spec = m_GraphicsContext->GetFramebuffer()->GetSpecification();
		return glm::ivec2(spec.Width, spec.Height);
	}

	std::string Engine::GetAssetsFolder() const
	{
		return m_AssetsFolder;
	}

	uint32_t Engine::GetWindowWidth() const
	{
		return m_GraphicsContext->GetWindowData()->Width;
	}

}
