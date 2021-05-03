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
		bool IsTetxureInPool(const std::string& filePath);
		bool IsMeshInPool(const std::string& filePath);
		void RenderScene(const glm::mat4& view, const glm::mat4& proj, const glm::vec3 camPos,
			float zNear, float zFar, bool debugDrawEnabled,
			CameraComponent* target = nullptr, TransformComponent* tranform = nullptr);

		Ref<Frostium::Mesh> AddOrGetMeshFromPool(const std::string& path);
		Ref<Frostium::Texture> AddOrGetTextureFromPool(const std::string& path);

		void Reload2DTextures(entt::registry& registry, const std::unordered_map<std::string, std::string>& assetMap);
		void Reload2DAnimations(entt::registry& registry);
		void ReloadAudioClips(entt::registry& registry, AudioEngine* engine);
		void ReloadCanvases(entt::registry& registry);
		void ReloadMeshMaterials(entt::registry& registry, SceneData* data);
		bool LoadStaticComponents();
		bool ChangeActorName(Actor* actor, const std::string& name);

		bool LoadMeshComponent(MeshComponent* component, const std::string& filePath, bool reset = true);
		bool SetMeshMaterial(MeshComponent* component, const Frostium::Mesh* target_mesh, Frostium::MaterialCreateInfo* info, const std::string& material_path);

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