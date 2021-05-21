#pragma once
#include "Core.h"
#include "Core/LayerManager.h"

#include <memory>
#include <functional>
#include <Frostium3D/Common/Events.h>
#include <Frostium3D/GraphicsContext.h>

namespace SmolEngine
{
	struct Physics2DContextCreateInfo
	{
		glm::vec2 GravityDir = glm::vec2(0.0f, -9.81f);
	};

	class WorldAdmin;
	class Scene;
	class ScriptingSystem;
	class Engine
	{
	public:
		virtual ~Engine();
		Engine();

		void                         Init();
		void                         Shutdown();
									 
		// Methods to implement		 
		virtual void                 SetGraphicsContext(Frostium::GraphicsContextInitInfo* info) {};
		virtual void                 SetPhysics2DContext(Physics2DContextCreateInfo* info) {};
		virtual void                 SetLayers(LayerManager* layerManager) {};
		virtual void                 SetScripts(ScriptingSystem* scriptingSytem) {};
									 
		// Getters					 
		inline static Engine*        GetEngine() { return s_Instance; }
		Frostium::GraphicsContext*   GetGraphicsContext() const;
		const uint32_t               GetWindowHeight() const;
		const uint32_t               GetWindowWidth() const;
		inline Frostium::Window*     GetWindow();
									 
		// Callbacks				 
		void                         SetOnSceneLoadedCallback(const std::function<void(Scene*)>& callback);
		void                         SetOnSceneUnLoadedCallback(const std::function<void(Scene*)>& callback);
									 
	private:						 
		void                         Run();
		// Events					 
		void                         OnWindowClose(Frostium::Event& e);
		void                         OnEvent(Frostium::Event& event);

	private:
		static Engine*               s_Instance;
		bool                         m_Running = false;
		Frostium::GraphicsContext*   m_GraphicsContext = nullptr;
		WorldAdmin*                  m_World = nullptr;
		LayerManager*                m_LayerHandler = nullptr;
		ScriptingSystem*             m_ScriptingSystem = nullptr;

		std::function<void(Scene*)>  m_SceneLoadCl = nullptr;
		std::function<void(Scene*)>  m_SceneUnLoadCl = nullptr;
	};

	Engine*                          CreateEngineContext();

}

