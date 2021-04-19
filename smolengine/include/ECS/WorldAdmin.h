#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <entt/entt.hpp>

#include <Frostium3D/Common/Time.h>
#include <Frostium3D/Common/Events.h>

namespace Frostium
{
	class Mesh;
	class Texture;
}

namespace SmolEngine
{ 
	struct WorldAdminStateSComponent;
	struct CameraComponent;
	struct TransformComponent;
	struct BehaviourComponent;
	struct SystemInstance;
	struct SceneData;
	class Scene;
	class AudioEngine;

	class WorldAdmin
	{
	public:

		WorldAdmin();

		void Init();
		void ShutDown();

		void OnEndWorld();
		void OnBeginWorld();
		void OnUpdate(Frostium::DeltaTime deltaTime);
		void OnEvent(Frostium::Event& e);
		void OnGameViewResize(float width, float height);

		void ReloadAssets();
		void CreateScene(const std::string& filePath);
		void AddScene(const std::string& filePath) {};
		bool Save(const std::string& filePath);
		bool Load(const std::string& filePath);
		bool SaveCurrentScene();
		bool IsInPlayMode();
		bool IsTetxureInPool(const std::string& filePath);
		bool IsMeshInPool(const std::string& filePath);

		void RenderScene(const glm::mat4& view, const glm::mat4& proj, const glm::vec3 camPos,
			float zNear, float zFar, bool debugDrawEnabled,
			CameraComponent* target = nullptr, TransformComponent* tranform = nullptr);

		Ref<Frostium::Mesh> AddOrGetMeshFromPool(const std::string& path);
		Ref<Frostium::Texture> AddOrGetTextureFromPool(const std::string& path);

		inline static WorldAdmin* GetSingleton() { return s_World; }
		Scene* GetActiveScene();

	private:

		void Reload2DTextures(entt::registry& registry, const std::unordered_map<std::string, std::string>& assetMap);
		void Reload2DAnimations(entt::registry& registry);
		void ReloadAudioClips(entt::registry& registry, AudioEngine* engine);
		void ReloadCanvases(entt::registry& registry);
		void ReloadMeshMaterials(entt::registry& registry, SceneData* data);
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