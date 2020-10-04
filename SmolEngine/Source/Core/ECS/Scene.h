#pragma once

#include "Core/ECS/SceneData.h"
#include "Core/ECS/Components.h"
#include "Core/ECS/Actor.h"

#include "Core/Renderer/Framebuffer.h"
#include "Core/Renderer/Renderer2D.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Camera.h"
#include "Core/EventHandler.h"

#include "Core/Audio/AudioEngine.h"
#include "Core/Application.h"

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <box2d/b2_world.h>
#include <Jinx.hpp>
#include <map>
#include <cereal/types/unordered_map.hpp>


namespace SmolEngine
{ 
	class SubTexture2D;
	class UILayer;

	class Scene
	{
	public:

		Scene() = default;

	   ~Scene() = default;

		//Main Logic

		void Init();

		void ShutDown();

		void OnPlay();

		void OnEndPlay();

		void OnUpdate(DeltaTime deltaTime);

		void OnEvent(Event& e);

		//Scene handling

		void Save(const std::string& filePath);

		void Load(const std::string& filePath);

		void SaveCurrentScene();

		void CreateScene(const std::string& filePath, const std::string& fileName);

		void AddScene(const std::string& filePath) {};

		void LoadSceneRuntime(uint32_t index) {};

		//Camera handling

		void UpdateEditorCamera(const glm::vec2& gameViewSize, const glm::vec2& sceneViewSize);

		void OnSceneViewResize(float width, float height);

		void OnGameViewResize(float width, float height);

		//Operations with actors

		void DeleteActor(Ref<Actor> actor);

		void DuplicateActor(Ref<Actor> actor);

		void AddChild(Ref<Actor> parent, Ref<Actor> child);

		void RemoveChild(Ref<Actor> parent, Ref<Actor> child);

		Ref<Actor> CreateActor(const std::string& name, const std::string& tag = std::string("Default"));

		Ref<Actor> FindActorByName(const std::string& name);

		Ref<Actor> FindActorByTag(const std::string& tag);

		Ref<Actor> FindActorByID(size_t id);

		std::vector<Ref<Actor>> GetActorListByTag(const std::string& tag);

		std::unordered_map<size_t, Ref<Actor>>& GetActorPool();

		std::vector <Ref<Actor>> GetActorList();

		std::vector <Ref<Actor>> GetSortedActorList();

		//Scripting

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

		bool AttachScript(const std::string& keyName, const Ref<Actor> actor);

		//Internal needs

		bool PathCheck(std::string& path, const std::string& fileName);

		const Jinx::RuntimePtr GetJinxRuntime();

		const std::unordered_map<std::string, size_t>& GetIDSet() { return m_IDSet; }

		static Ref<Scene> GetScene() { return s_Scene; }

		b2World* GetWorld() { return m_World; }

		entt::registry& GetRegistry() { return m_SceneData.m_Registry; }

	private:

		bool ChangeFilePath(const std::string& fileName, std::string& pathToChange);
		bool IsPathValid(const std::string& path);

		SceneData& GetSceneData();

	private:

		SceneData m_SceneData;

		std::map<std::string, Ref<ScriptableObject>> m_ScriptRegistry;
		std::unordered_map<std::string, size_t> m_IDSet;
		std::vector<std::string> m_ScriptNameList;

		static Ref<Scene> s_Scene;

		Jinx::RuntimePtr m_JinxRuntime = nullptr;

		AudioEngine* m_AudioEngine = nullptr;
		UILayer* m_UILayer = nullptr;

		Ref<SubTexture2D> m_TestSub = nullptr;
		Ref<EditorCameraController> m_EditorCamera = nullptr;

		bool m_InPlayMode = false;
		b2World* m_World = nullptr;

	private:

		friend class EditorLayer;
		friend class Actor;
		friend class SettingsWindow;
		friend class AnimationPanel;
		friend class Animation2DController;
	};
}