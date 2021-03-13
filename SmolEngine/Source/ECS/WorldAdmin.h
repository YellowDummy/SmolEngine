#pragma once
#include "Core/Core.h"
#include "Core/Time.h"
#include "Core/EventHandler.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <entt.hpp>

namespace SmolEngine
{ 
	struct WorldAdminStateSComponent;
	struct CameraComponent;
	struct TransformComponent;
	struct BehaviourComponent;
	struct SystemInstance;

	class SubTexture2D;
	class EditorCameraController;
	class Framebuffer;
	class EditorCamera;
	class Mesh;
	class Texture;
	class Scene;

	class WorldAdmin
	{
	public:

		WorldAdmin();

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

		void RenderScene(const glm::mat4& view, const glm::mat4& proj, const glm::vec3 camPos,
			float zNear, float zFar, bool debugDrawEnabled,
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

		bool IsInPlayMode();

		bool IsTetxureInPool(const std::string& filePath);

		bool IsMeshInPool(const std::string& filePath);

		// Helpers

		Ref<Mesh> AddOrGetMeshFromPool(const std::string& path);

		Ref<Texture> AddOrGetTextureFromPool(const std::string& path);

		// Camera handling
		
		void UpdateEditorCamera(Ref<EditorCamera>& cam);

		void OnGameViewResize(float width, float height);

		// Getters

		inline static WorldAdmin* GetSingleton() { return s_World; }

		Scene* GetActiveScene();

	private:

		// Internal needs

		bool LoadProjectConfig();

		bool LoadStaticComponents();

	private:

		inline static WorldAdmin*            s_World = nullptr;
		WorldAdminStateSComponent*           m_State = nullptr;

		entt::registry                       m_GlobalRegistry;

	private:

		friend class Animation2DController;
		friend class SettingsWindow;
		friend class AnimationPanel;
		friend class EditorLayer;
		friend class Actor;
	};
}