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
	struct MaterialCreateInfo;
	struct BeginSceneInfo;
}

namespace SmolEngine
{ 
	struct WorldAdminStateSComponent;
	struct CameraComponent;
	struct TransformComponent;
	struct BehaviourComponent;
	struct SystemInstance;
	struct SceneData;
	struct MeshComponent;
	class Scene;
	class AudioEngine;

	class WorldAdmin
	{
	public:

		WorldAdmin();

		bool SaveCurrentScene();
		bool CreateScene(const std::string& filePath);
		bool SaveScene(const std::string& filePath);
		bool LoadScene(const std::string& filePath);
		bool LoadSceneAtIndex(const std::string& filePath, uint32_t index);
		bool LoadSceneBG(const std::string& filePath);
		bool SwapScene(uint32_t index);

		// Setters
		void SetBeginSceneInfo(Frostium::BeginSceneInfo* info);

		// Getters
		inline static WorldAdmin* GetSingleton() { return s_World; }
		Scene* GetActiveScene();

	private:

		void Init();
		void ShutDown();
		void ReloadAssets();

		void OnEndWorld();
		void OnBeginWorld();
		void OnBeginFrame();
		void OnEndFrame();
		void OnUpdate(Frostium::DeltaTime deltaTime);
		void OnEvent(Frostium::Event& e);
		void OnGameViewResize(float width, float height);

		bool IsInPlayMode();
		bool LoadStaticComponents();
		bool ChangeActorName(Actor* actor, const std::string& name);

		void Reload2DTextures(entt::registry& registry);
		void Reload2DAnimations(entt::registry& registry);
		void ReloadAudioClips(entt::registry& registry, AudioEngine* engine);
		void ReloadCanvases(entt::registry& registry);
		void ReloadMeshes(entt::registry& registry);
		void ReloadRigidBodies(entt::registry& registry);

	private:

		inline static WorldAdmin*    s_World = nullptr;
		WorldAdminStateSComponent*   m_State = nullptr;
		entt::registry               m_GlobalRegistry{};
		entt::entity                 m_GlobalEntity{};

	private:

		friend class Animation2DController;
		friend class SettingsWindow;
		friend class AnimationPanel;
		friend class EditorLayer;
		friend class Actor;
		friend class Engine;
	};
}