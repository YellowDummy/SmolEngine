#pragma once

#include "ECS/Scene.h"
#include "Core/EventHandler.h"
#include "Audio/AudioEngine.h"
#include "Core/Application.h"

#include "ECS/ComponentTuples/BaseTuple.h"

#include <string>
#include <utility>
#include <vector>
#include <type_traits>
#include <functional>
#include <box2d/b2_world.h>
#include <map>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{ 
	struct CameraComponent;
	struct TransformComponent;
	struct BehaviourComponent;
	struct SystemInstance;

	class SubTexture2D;
	class EditorCameraController;
	class Framebuffer;
	class EditorCamera;

	class WorldAdmin
	{
	public:

		// Initialization

		void Init();

		void ShutDown();

		void StartGame();
 
		// Simulation

		void OnEndPlay();

		void OnPlay();

		// Update-Loop
		
		void OnUpdate(DeltaTime deltaTime);

		void OnEvent(Event& e);

		// Rendering

		void RenderScene(const glm::mat4& viewProjectionMatrix, Ref<Framebuffer> framebuffer, bool debugDrawEnabled, 
			CameraComponent* target = nullptr, TransformComponent* tranform = nullptr);

		// Assets

		void ReloadAssets();

		// Scene handling

		void CreateScene(const std::string& filePath);

		void AddScene(const std::string& filePath) {};

		bool Save(const std::string& filePath);

		bool Load(const std::string& filePath);

		bool LoadSceneRuntime(uint32_t index);

		bool SaveCurrentScene();

		// Camera handling
		
		void UpdateEditorCamera(Ref<EditorCamera>& cam);

		void OnGameViewResize(float width, float height);

		// Getters

		inline static Ref<WorldAdmin>& GetSingleton() { return s_World; }

		Scene& GetActiveScene();

		// Internal needs

		bool LoadProjectConfig();

	private:

		inline static Ref<WorldAdmin>        s_World = std::make_shared<WorldAdmin>();
		Ref<EditorCameraController>          m_EditorCamera = nullptr;
		Ref<SubTexture2D>                    m_TestSub = nullptr;
		bool                                 m_InPlayMode = false;
		size_t                               m_ActiveSceneID = 0;
		std::unordered_map<size_t, Scene>    m_SceneMap;

	private:

		friend class Animation2DController;
		friend class SettingsWindow;
		friend class AnimationPanel;
		friend class EditorLayer;
		friend class Actor;
	};
}