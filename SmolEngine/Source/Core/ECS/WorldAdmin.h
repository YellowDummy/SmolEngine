#pragma once

#include "Core/ECS/Scene.h"
#include "Core/EventHandler.h"

#include "Core/ECS/ComponentTuples/BaseTuple.h"

#include "Core/Audio/AudioEngine.h"
#include "Core/Application.h"

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
	class SubTexture2D;
	class EditorCameraController;
	class Framebuffer;

	struct CameraComponent;
	struct TransformComponent;
	struct BehaviourComponent;
	struct SystemInstance;

	class WorldAdmin
	{
	public:

		WorldAdmin()  = default;

	    ~WorldAdmin() = default;

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
		
		void UpdateEditorCamera(const glm::vec2& gameViewSize, const glm::vec2& sceneViewSize);

		void OnSceneViewResize(float width, float height);

		void OnGameViewResize(float width, float height);

		// Getters

		inline static Ref<WorldAdmin>& GetSingleton() { return s_World; }

		Scene& GetActiveScene();

		// Internal needs

		bool LoadProjectConfig();

	private:

		inline static Ref<WorldAdmin> s_World = std::make_shared<WorldAdmin>();

		std::unordered_map<size_t, Scene> m_SceneMap;
		size_t m_ActiveSceneID = 0;

		Ref<EditorCameraController> m_EditorCamera = nullptr;
		Ref<SubTexture2D> m_TestSub = nullptr;
		bool m_InPlayMode = false;

	private:

		friend class Animation2DController;
		friend class SettingsWindow;
		friend class AnimationPanel;
		friend class EditorLayer;
		friend class Actor;
	};
}