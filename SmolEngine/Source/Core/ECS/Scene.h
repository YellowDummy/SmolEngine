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

#include <box2d/b2_world.h>

namespace SmolEngine
{
	struct SceneData
	{
		std::vector<Ref<Actor>> m_ActorPool;
		b2Vec2 m_Gravity = b2Vec2(0, -9.81f);
	};

	class Scene
	{
	public:
		void Init();
		void ShutDown();

		void OnPlay();
		void OnEndPlay();

		void OnUpdate(DeltaTime deltaTime);
		void OnEvent(Event& e);

		void UpdateEditorCamera();

		void OnSceneViewResize(float width, float height);
		void OnGameViewResize(float width, float height);

		Ref<Actor> CreateActor(const std::string& name, const std::string& tag = std::string("Default"));
		void DeleteActor(const Ref<Actor> actor);
		void DeleteActorFromScene(const Ref<Actor> actor);

		void AddChild(Ref<Actor> parent, Ref<Actor> child);
		void RemoveChild(Ref<Actor> parent, Ref<Actor> child);

		Ref<Actor> FindActorByName(const std::string& name);
		Ref<Actor> FindActorByTag(const std::string& tag);

		std::vector<Ref<Actor>> GetActorListByTag(const std::string& tag);
		std::vector<Ref<Actor>>& GetActorPool();

		const Jinx::RuntimePtr GetJinxRuntime();
		const std::unordered_map<std::string, size_t>& GetIDSet() { return m_IDSet; }
		static Ref<Scene> GetScene() { return s_Scene; }
		b2World* GetWorld() { return m_World; }

	private:
		bool IsActorExist(Ref<Actor> actor);

	private:
		static Ref<Scene> s_Scene;
		bool m_InPlayMode = false;

		b2World* m_World;
		Jinx::RuntimePtr m_JinxRuntime;
		entt::registry m_Registry;
		Ref<EditorCameraController> m_EditorCamera;

		std::unordered_map<std::string, size_t> m_IDSet;

		friend class EditorLayer;
	};
}