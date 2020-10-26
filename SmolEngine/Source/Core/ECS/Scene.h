#pragma once

#include "Core/ECS/SceneData.h"
#include "Core/ECS/Actor.h"

#include "Core/Renderer/Framebuffer.h"
#include "Core/Renderer/Renderer2D.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Camera.h"
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

	struct CameraBaseTuple;

	///

	class Scene
	{
	public:

		Scene()  = default;

	    ~Scene() = default;

	    /// Initialization

		void StartGame();

		void ShutDown();

		void Init();

		/// Simulation

		void OnEndPlay();

		void OnPlay();

		/// Update-Loop

		void OnUpdate(DeltaTime deltaTime);

		void OnEvent(Event& e);

		/// Rendering

		void RenderScene(const glm::mat4& viewProjectionMatrix, CameraBaseTuple* target = nullptr);

		/// Assets

		bool AddAsset(const std::string& fileName, const std::string& filePath);

		bool DeleteAsset(const std::string& fileName);

		void ReloadAssets();

		void ReloadScripts();

		/// Scene handling

		void CreateScene(const std::string& filePath, const std::string& fileName);

		void AddScene(const std::string& filePath) {};

		bool Save(const std::string& filePath);

		bool Load(const std::string& filePath);

		bool LoadSceneRuntime(uint32_t index);

		bool SaveCurrentScene();

		/// Camera handling
		
		void UpdateEditorCamera(const glm::vec2& gameViewSize, const glm::vec2& sceneViewSize);

		void OnSceneViewResize(float width, float height);

		void OnGameViewResize(float width, float height);

		/// Operations with actors

		Ref<Actor> CreateActor(const ActorBaseType baseType, const std::string& name, const std::string& tag = std::string("Default"));

		void RemoveChild(Ref<Actor> parent, Ref<Actor> child);

		void AddChild(Ref<Actor> parent, Ref<Actor> child);

		Actor* FindActorByName(const std::string& name);

		Actor* FindActorByTag(const std::string& tag);

		Actor* FindActorByID(const uint32_t id);

		void DuplicateActor(Ref<Actor> actor);

		void DeleteActor(Ref<Actor> actor);

		/// Getters
		
		const std::unordered_map<std::string, std::string>& GetAssetMap();

		std::vector<Ref<Actor>> GetActorListByTag(const std::string& tag);

		entt::registry& GetRegistry() { return m_SceneData.m_Registry; }

		std::unordered_map<size_t, Ref<Actor>>& GetActorPool();

		static Ref<Scene> GetScene() { return s_Scene; }

		std::vector <Ref<Actor>> GetActorList();

		std::vector<Actor*> GetSortedActorList();

		SceneData& GetSceneData();

		/// Templates
		
		template<typename T>
		void ForEachTuple(std::function< void(T&) > callback)
		{
			const auto& view = m_SceneData.m_Registry.view<T>();
			for (const auto& entity : view)
			{
				auto& tuple = view.get<T>(entity);
				callback(tuple);
			}
		}

		template<typename T>
		void RegistryScript(const std::string& keyName)
		{
			auto result = m_ScriptRegistry.find(keyName);
			if (result != m_ScriptRegistry.end())
			{
				NATIVE_ERROR("<{}> script is already registered!", keyName); return;
			}

			std::shared_ptr<ScriptableObject> obj = std::make_shared<T>();
			m_ScriptNameList.push_back(keyName);
			m_ScriptRegistry[keyName] = obj;
		}

		template<typename T>
		T* AddTuple(Actor& actor)
		{
			return static_cast<T*>(ProcessTuple<T>(actor));
		}

		// Don't use! For internal needs
		template<typename T>
		BaseTuple* ProcessTuple(Actor& actor)
		{
			if (HasTuple<T>(actor))
			{
				NATIVE_ERROR("Actor already have tuple!");
				return nullptr;
			}

			if (std::is_same<T, ResourceTuple>::value)
			{
				//
			}

			auto& component = m_SceneData.m_Registry.emplace<T>(actor);
			return &component;
		}

		template<typename T>
		bool HasTuple(entt::entity entity)
		{
			return m_SceneData.m_Registry.has<T>(entity);
		}

		template<typename T>
		T* GetTuple(entt::entity entity)
		{
			if (!HasTuple<T>(entity))
			{
				NATIVE_ERROR("Actor does not have tuple");
				return nullptr;
			}

			auto& component = m_SceneData.m_Registry.get<T>(entity);
			return &component;
		}

		/// Scripting

		bool AttachScript(const std::string& keyName, const Ref<Actor> actor);

		/// Internal needs
		
		const std::unordered_map<std::string, size_t>& GetIDSet() { return m_IDSet; }

		bool UpdateIDSet(const std::string& lastName, const std::string& newName);

		bool PathCheck(std::string& path, const std::string& fileName);

		bool LoadProjectConfig();

		void DeleteSingletons();

		void LoadSingletons();

		///

	private:

		bool ChangeFilePath(const std::string& fileName, std::string& pathToChange);

		bool IsPathValid(const std::string& path);

	private:

		SceneData m_SceneData;

		///

		std::map<std::string, Ref<ScriptableObject>> m_ScriptRegistry;

		std::unordered_map<std::string, size_t> m_IDSet;

		std::vector<std::string> m_ScriptNameList;

		///

		Ref<EditorCameraController> m_EditorCamera = nullptr;

		Ref<SubTexture2D> m_TestSub = nullptr;

		static Ref<Scene> s_Scene;

		///

		bool m_InPlayMode = false;

	private:

		friend class Animation2DController;

		friend class SettingsWindow;

		friend class AnimationPanel;

		friend class EditorLayer;

		friend class Actor;
	};
}