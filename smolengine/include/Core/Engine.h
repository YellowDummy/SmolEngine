#pragma once
#include "Core.h"
#include "Core/LayerManager.h"
#include "ECS/Systems/ScriptingSystem.h"

#include <memory>
#include <functional>
#include <Frostium3D/Common/Events.h>
#include <Frostium3D/GraphicsContext.h>

namespace SmolEngine
{
	struct PhysicsContextCreateInfo
	{

	};

	class WorldAdmin;
	class Engine
	{
	public:
		virtual ~Engine();
		Engine();

		void Init();
		void Shutdown();

		// Methods to implement
		virtual void SetGraphicsContext(Frostium::GraphicsContextInitInfo* info) {};
		virtual void SetPhysicsContext(PhysicsContextCreateInfo* info) {};
		virtual void SetLayers(LayerManager* layerManager) {};
		virtual void SetScripts(ScriptingSystem* scriptingSytem) {};

		// Getters
		inline static Engine* GetEngine() { return s_Instance; }
		Frostium::GraphicsContext* GetGraphicsContext() const;
		const uint32_t GetWindowHeight() const;
		const uint32_t GetWindowWidth() const;
		inline Frostium::Window* GetWindow();

	private:
		void Run();
		// Events
		void OnWindowClose(Frostium::Event& e);
		void OnEvent(Frostium::Event& event);

	private:
		static Engine*               s_Instance;
		Frostium::GraphicsContext*   m_GraphicsContext = nullptr;
		WorldAdmin*                  m_World = nullptr;
		LayerManager*                m_LayerHandler = nullptr;
		ScriptingSystem*             m_ScriptingSystem = nullptr;
		bool                         m_Running = false;
	};

	Engine* CreateEngineContext();

}

