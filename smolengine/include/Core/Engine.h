#pragma once
#include "Core.h"
#include "Core/LayerManager.h"

#include <memory>
#include <functional>
#ifndef FROSTIUM_SMOLENGINE_IMPL
#define FROSTIUM_SMOLENGINE_IMPL
#endif
#include <Frostium3D/Common/Events.h>
#include <Frostium3D/GraphicsContext.h>

namespace SmolEngine
{
	struct PhysicsModuleCreateInfo
	{
		glm::vec2 GravityDir = glm::vec2(0.0f, -9.81f);
	};

	struct EngineModuleCreateInfo
	{
		std::string AssetsFolder = "../samples/";
	};

	struct WorldAdminStateSComponent;
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
		virtual void                 OnEngineModuleCreation(EngineModuleCreateInfo* info) {}
		virtual void                 OnGraphicsModuleCreation(GraphicsContextInitInfo* info) {}
		virtual void                 OnPhysicsModuleCreation(PhysicsModuleCreateInfo* info) {}
		virtual void                 OnLayerModuleCreation(LayerManager* layerManager) {}
		virtual void                 OnWorldAdminModuleCreation(WorldAdminStateSComponent* state) {}
		virtual void                 OnScriptModuleCreation(ScriptingSystem* system) {}
		virtual void                 OnInitializationComplete(WorldAdmin* admin) {}
									 
		// Getters					 
		inline static Engine*        GetEngine() { return s_Instance; }
		GraphicsContext*             GetGraphicsContext() const;
		uint32_t                     GetWindowHeight() const;
		uint32_t                     GetWindowWidth() const;
		float                        GetFPSCount() const;
		glm::ivec2                   GetViewPortSize() const;
		std::string                  GetAssetsFolder() const;
									 
		// Callbacks				 
		void                         SetOnSceneLoadedCallback(const std::function<void(Scene*)>& callback);
		void                         SetOnSceneUnLoadedCallback(const std::function<void(Scene*)>& callback);
									 
	private:						 

		void                         Run();
		void                         OnWindowClose(Event& e);
		void                         OnEvent(Event& event);

	private:
		static Engine*               s_Instance;
		bool                         m_Running = false;
		float                        m_FPSCount = 0.0f;
		GraphicsContext*             m_GraphicsContext = nullptr;
		WorldAdmin*                  m_World = nullptr;
		LayerManager*                m_LayerHandler = nullptr;
		std::string                  m_AssetsFolder;
		std::function<void(Scene*)>  m_SceneLoadCl = nullptr;
		std::function<void(Scene*)>  m_SceneUnLoadCl = nullptr;
	};
}

SmolEngine::Engine* CreateEngineContext();

