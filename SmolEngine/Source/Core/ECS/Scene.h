#pragma once

#include <string>
#include <utility>
#include <vector>
#include <functional>

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
#include <functional>
#include <glm/glm.hpp>

#include <box2d/b2_world.h>

namespace SmolEngine
{
	struct SceneData
	{
		SceneData() = default;

		entt::registry m_Registry;
		std::vector<Ref<Actor>> m_ActorPool;
		glm::vec2 Gravity = glm::vec2(0.0f, -9.81f);

		void operator=(const SceneData& other)
		{
			m_ActorPool = other.m_ActorPool;
			Gravity.x = other.Gravity.x;
			Gravity.y = other.Gravity.y;
			m_ID = other.m_ID;
			m_filePath = other.m_filePath;
			m_Name = other.m_Name;
		}

	private:
		friend class EditorLayer;
		friend class Scene;
		friend class SettingsWindow;
		friend class cereal::access;

		std::string m_filePath;
		std::string m_Name = std::string("");
		size_t m_ID = 0;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_ActorPool, Gravity.x, Gravity.y, m_ID, m_filePath, m_Name);
			archive.serializeDeferments();
		}
	};

	class Scene
	{
	public:

		Scene() = default;

		void Init();
		void ShutDown();

		void OnPlay();
		void OnEndPlay();

		void OnUpdate(DeltaTime deltaTime);
		void OnEvent(Event& e);

		void Save(std::string& filePath);
		void Load(std::string& filePath);
		void CreateScene(std::string& filePath);

		void UpdateEditorCamera();
		void OnSceneViewResize(float width, float height);
		void OnGameViewResize(float width, float height);

		void DeleteActor(const Ref<Actor> actor);
		void DeleteActorFromScene(const Ref<Actor> actor);

		void AddChild(Ref<Actor> parent, Ref<Actor> child);
		void RemoveChild(Ref<Actor> parent, Ref<Actor> child);

		Ref<Actor> CreateActor(const std::string& name, const std::string& tag = std::string("Default"));
		Ref<Actor> FindActorByName(const std::string& name);
		Ref<Actor> FindActorByTag(const std::string& tag);
		Ref<Actor> FindActorByID(size_t id);

		std::vector<Ref<Actor>> GetActorListByTag(const std::string& tag);
		std::vector<Ref<Actor>>& GetActorPool();

		const Jinx::RuntimePtr GetJinxRuntime();
		const std::unordered_map<std::string, size_t>& GetIDSet() { return m_IDSet; }

		static Ref<Scene> GetScene() { return s_Scene; }

		b2World* GetWorld() { return m_World; }
		entt::registry& GetRegistry() { return s_SceneData.m_Registry; }

		template<typename T>
		void SetScriptToActor(Ref<Actor> actor)
		{
			auto& ref = actor->AddComponent<ScriptComponent>();
			ref.SetScript<T>(actor, s_SceneData.m_ID);

			m_Scripts.push_back(ref);
		}

	private:
		bool IsActorExist(Ref<Actor> actor);
		SceneData& GetSceneData();

	private:
		static Ref<Scene> s_Scene;

		SceneData s_SceneData;

		bool m_InPlayMode = false;

		b2World* m_World = nullptr;
		Jinx::RuntimePtr m_JinxRuntime;

		Ref<EditorCameraController> m_EditorCamera;

		std::vector<ScriptComponent> m_Scripts;
		std::unordered_map<std::string, size_t> m_IDSet;

		friend class EditorLayer;
		friend class Actor;
		friend class SettingsWindow;
	};
}