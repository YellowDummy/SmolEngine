#pragma once

#include "Core/ECS/SceneData.h"
#include "Core/EventHandler.h"

#include "Core/ECS/ComponentTuples/BaseTuple.h"
#include "Core/ECS/Actor.h"
#include "Core/Scripting/SystemRegistry.h"

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

	struct CameraBaseTuple;

	struct BehaviourComponent;

	struct SystemInstance;

	///

	class WorldAdmin
	{
	public:

		WorldAdmin()  = default;

	    ~WorldAdmin() = default;

	    /// 
		/// Initialization
		/// 

		void StartGame();

		void ShutDown();

		void Init();

		void InitSystems();

		/// 
		/// Simulation
		/// 

		void OnEndPlay();

		void OnPlay();

		/// 
		/// Update-Loop
		/// 

		void OnUpdate(DeltaTime deltaTime);

		void OnEvent(Event& e);

		/// 
		/// Systems Processing
		/// 

		void OnSystemBegin();

		void OnSystemTick(DeltaTime deltaTime);

		void PrepareSystem(const BehaviourComponent& behaviour, const SystemInstance& sysRef);

		/// 
		/// Rendering
		/// 

		void RenderScene(const glm::mat4& viewProjectionMatrix, Ref<Framebuffer> framebuffer, CameraBaseTuple* target = nullptr);

		/// 
		/// Assets
		/// 

		bool AddAsset(const std::string& fileName, const std::string& filePath);

		bool DeleteAsset(const std::string& fileName);

		void ReloadAssets();

		void ReloadScripts();

		/// 
		/// Scene handling
		/// 

		void CreateScene(const std::string& filePath, const std::string& fileName);

		void AddScene(const std::string& filePath) {};

		bool Save(const std::string& filePath);

		bool Load(const std::string& filePath);

		bool LoadSceneRuntime(uint32_t index);

		bool SaveCurrentScene();

		/// 
		/// Camera handling
		/// 
		
		void UpdateEditorCamera(const glm::vec2& gameViewSize, const glm::vec2& sceneViewSize);

		void OnSceneViewResize(float width, float height);

		void OnGameViewResize(float width, float height);

		/// 
		/// Operations with actors
		/// 

		Ref<Actor> CreateActor(const ActorBaseType baseType, const std::string& name, const std::string& tag = std::string("Default"));

		BehaviourComponent* AddBehaviour(const std::string& systemName, const Ref<Actor>& actor);

		void RemoveChild(Ref<Actor>& parent, Ref<Actor>& child);

		void AddChild(Ref<Actor>& parent, Ref<Actor>& child);

		Ref<Actor> FindActorByName(const std::string& name);
		
		Ref<Actor> FindActorByTag(const std::string& tag);
		
		Ref<Actor> FindActorByID(const uint32_t id);

		void DuplicateActor(Ref<Actor>& actor);

		void DeleteActor(Ref<Actor>& actor);

		/// 
		/// Getters
		/// 
		
		std::unordered_map<std::string, SystemInstance>& GetSystemMap() { return m_SystemMap; }

		const std::unordered_map<std::string, std::string>& GetAssetMap();

		std::vector<Ref<Actor>> GetActorListByTag(const std::string& tag);

		entt::registry& GetRegistry() { return m_SceneData.m_Registry; }

		std::unordered_map<size_t, Ref<Actor>>& GetActorPool();

		static Ref<WorldAdmin> GetScene() { return s_Scene; }

		std::vector <Ref<Actor>> GetActorList();

		std::vector<Ref<Actor>> GetSortedActorList();

		SceneData& GetSceneData();

		/// 
		/// Templates
		/// 

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
				if (HasTuple<CameraBaseTuple>(actor))
				{
					return nullptr;
				}
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

		/// 
		/// Internal needs
		/// 
		
		const std::unordered_map<std::string, size_t>& GetIDSet() { return m_IDSet; }

		bool OnActorNameChanged(const std::string& lastName, const std::string& newName);

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

		std::unordered_map<std::string, SystemInstance> m_SystemMap;

		std::unordered_map<std::string, size_t> m_IDSet;

		///

		Ref<EditorCameraController> m_EditorCamera = nullptr;

		Ref<SubTexture2D> m_TestSub = nullptr;

		static Ref<WorldAdmin> s_Scene;

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