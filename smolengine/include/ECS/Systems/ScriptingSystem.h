#pragma once
#include "Core/Core.h"
#include "Scripting/BehaviourPrimitive.h"
#include "ECS/Components/Singletons/ScriptingSystemStateSComponent.h"

#include <meta/meta.hpp>
#include <Frostium3D/Common/Time.h>

namespace SmolEngine
{
	class Actor;
	class ScriptingSystem
	{
	public:

		// Factory
		template<typename T>
		bool AddNativeClass(const std::string& name)
		{
			ScriptingSystemStateSComponent* instance = ScriptingSystemStateSComponent::GetSingleton();
			if (instance == nullptr)
				return false;

			auto& it = instance->MetaMap.find(name);
			if (it != instance->MetaMap.end())
				return false;

			auto& hash = instance->Hash;
			// Hashes
			size_t nameHash = hash(name);
			size_t onBeginHash = hash("OnBegin");
			size_t onProcessHash = hash("OnProcess");
			size_t onDestroyHash = hash("OnDestroy");
			size_t onCollBeginHash = hash("OnCollisionContact");
			size_t onCollExitHash = hash("OnCollisionExit");
			size_t onDebugDrawHash = hash("OnDebugDraw");
			// Registry Part
			auto factory = meta::reflect<T>(nameHash);
			factory.base<BehaviourPrimitive>();
			factory.func<&T::OnBegin>(onBeginHash);
			factory.func<&T::OnProcess>(onProcessHash);
			factory.func<&T::OnDestroy>(onDestroyHash);
			factory.func<&T::OnCollisionContact>(onCollBeginHash);
			factory.func<&T::OnCollisionExit>(onCollExitHash);
			factory.func<&T::OnDebugDraw>(onDebugDrawHash);

			// Reflection Part
			ScriptingSystemStateSComponent::MetaData metaData = {};
			metaData.ClassInstance = T();
			metaData.OnBeginFunc = meta::resolve(nameHash).func(onBeginHash);
			metaData.OnDestroyFunc = meta::resolve(nameHash).func(onDestroyHash);
			metaData.OnProcessFunc = meta::resolve(nameHash).func(onProcessHash);
			metaData.OnCollBeginFunc = meta::resolve(nameHash).func(onCollBeginHash);
			metaData.OnCollEndFunc = meta::resolve(nameHash).func(onCollExitHash);
			metaData.OnDebugDrawFunc = meta::resolve(nameHash).func(onDebugDrawHash);

			instance->MetaMap[name] = std::move(metaData);
			return true;
		}

	private:

		static bool AttachNativeScript(Actor* actor, const std::string& scriptName);

		static void OnBegin();
		static void OnEnd();
		static void OnDestroy(Actor* actor);

		static void OnTick(Frostium::DeltaTime deltaTime);
		static void OnCollisionBegin(Actor* actorB, Actor* actorA, bool isTrigger);
		static void OnCollisionEnd(Actor* actorB, Actor* actorA, bool isTrigger);
		static void OnDebugDraw();

	private:

		static void ReloadScripts();

	private:

		friend class WorldAdmin;
		friend class CollisionListener2D;

	};
}