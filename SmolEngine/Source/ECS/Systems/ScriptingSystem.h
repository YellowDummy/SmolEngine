#pragma once
#include "Core/Core.h"
#include "Core/Time.h"

#include <entt.hpp>

namespace SmolEngine
{
	class Actor;

	class ScriptingSystem
	{
	public:

		static void OnSceneBegin(entt::registry& registry);

		static void OnSceneEnd(entt::registry& registry);

		static void OnSceneTick(entt::registry& registry, DeltaTime deltaTime);

		static void OnDestroy(Ref<Actor>& actor);

		static void OnCollisionBegin(Actor* actorB, Actor* actorA, bool isTrigger);

		static void OnCollisionEnd(Actor* actorB, Actor* actorA, bool isTrigger);

		static void OnDebugDraw(entt::registry& registry);

		static void ReloadScripts(entt::registry& registry, const std::unordered_map<size_t, Ref<Actor>>& actorPool);
	};
}