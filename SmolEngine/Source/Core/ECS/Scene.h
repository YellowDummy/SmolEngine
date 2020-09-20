#pragma once

#include <string>
#include <utility>
#include <vector>
#include <functional>

#include <box2d/b2_world.h>

#include "Core/ECS/Components.h"
#include "Core/ECS/Actor.h"

#include "Core/Renderer/Framebuffer.h"
#include "Core/Renderer/Renderer2D.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Camera.h"
#include "Core/EventHandler.h"
#include "Core/Application.h"

#include <Jinx.hpp>
#include <unordered_map>
#include <glm/glm.hpp>


namespace SmolEngine
{ 
	class SubTexture2D;

	//TODO: Move to a separate header
	struct SceneData
	{
		entt::registry m_Registry;
		//TODO: Replace vector
		std::vector<Ref<Actor>> m_ActorPool;
		glm::vec2 m_Gravity = glm::vec2(0.0f, -9.81f);

		float m_AmbientStrength = 1.0f;

	public:
		SceneData() = default;
		void operator=(const SceneData& other);

	private:
		friend class EditorLayer;
		friend class Scene;
		friend class SettingsWindow;
		friend class cereal::access;

		std::string m_filePath;
		std::string m_fileName;

		std::string m_Name = std::string("");
		size_t m_ID = 0;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_ActorPool, m_Gravity.x, m_Gravity.y, m_ID, m_filePath, m_fileName, m_Name, m_AmbientStrength);
			archive.serializeDeferments();
		}
	};

	class Scene
	{
	public:

		Scene() = default;

		// Logic
		void Init();
		void ShutDown();

		void OnPlay();
		void OnEndPlay();

		void OnUpdate(DeltaTime deltaTime);
		void OnEvent(Event& e);

		//Serilization
		void Save(std::string& filePath);
		void Load(std::string& filePath);
		void Load(int index) {}
		void SaveCurrentScene();
		void CreateScene(const std::string& filePath, const std::string& fileName);

		//Camera handling
		void UpdateEditorCamera();
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
		std::vector<Ref<Actor>>& GetActorPool();

		//Scripting
		template<typename T>
		void RegistryScript(std::string& keyName)
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

		bool AttachScript(std::string& keyName, Ref<Actor> actor);

		//Internal needs
		bool PathCheck(std::string& path, const std::string& fileName);

		const Jinx::RuntimePtr GetJinxRuntime();

		const std::unordered_map<std::string, size_t>& GetIDSet() { return m_IDSet; }

		static Ref<Scene> GetScene() { return s_Scene; }

		b2World* GetWorld() { return m_World; }

		entt::registry& GetRegistry() { return s_SceneData.m_Registry; }

	private:
		bool IsActorExist(Ref<Actor> actor);
		bool ChangeFilePath(const std::string& fileName, std::string& pathToChange);
		bool IsPathValid(const std::string& path);

		SceneData& GetSceneData();

	private:
		friend class EditorLayer;
		friend class Actor;
		friend class SettingsWindow;
		friend class AnimationPanel;
		friend class Animation2DController;

	private:
		static Ref<Scene> s_Scene;

		SceneData s_SceneData;

		bool m_InPlayMode = false;

		b2World* m_World = nullptr;
		Jinx::RuntimePtr m_JinxRuntime;

		Ref<SubTexture2D> m_TestSub = nullptr;
		Ref<EditorCameraController> m_EditorCamera;

		std::unordered_map<std::string, Ref<ScriptableObject>> m_ScriptRegistry;
		std::unordered_map<std::string, size_t> m_IDSet;
		std::vector<std::string> m_ScriptNameList;

	};
}