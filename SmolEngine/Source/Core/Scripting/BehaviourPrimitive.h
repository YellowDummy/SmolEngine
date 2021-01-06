#pragma once

#include "Core/Scripting/OutValues.h"

#include <string>
#include <any>
#include <rttr/registration.h>
#include <rttr/type.h>
#include <rttr/registration_friend>

namespace SmolEngine
{
	class Actor;

	//Note:
	//Base class for all script-systems

	class BehaviourPrimitive
	{
	public:

		BehaviourPrimitive() = default;

		virtual ~BehaviourPrimitive() = default;

		/// Methods to implement

		virtual void OnCollisionContact(Actor* another, bool isTrigger) {}

		virtual void OnCollisionExit(Actor* another, bool isTrigger) {}

		virtual void OnDebugDraw() {}

		/// Getters

		const std::string& GetName();

		const std::string& GetTag();

		const size_t GetID();

		template<typename T>
		T* GetComponent()
		{
			return WorldAdmin::GetScene()->GetComponent<T>(*m_Actor);
		}

		/// Search

		Ref<Actor> FindActorByName(const std::string& name);

		Ref<Actor> FindActorByTag(const std::string& tag);

		Ref<Actor> FindActorByID(size_t id);

		/// Out-Properties

		void PushOutProperty(const char* keyName, std::any val, OutValueType type);

		/// Defines

#define OUT_FLOAT(name, value) PushOutProperty(name, value, OutValueType::Float)
#define OUT_INT(name, value) PushOutProperty(name, value, OutValueType::Int)
#define OUT_STRING(name, value) PushOutProperty(name, value, OutValueType::String)

	private:

		Ref<Actor> m_Actor = nullptr;

		std::unordered_map<const char*, float*> m_OutFloatVariables;
		std::unordered_map<const char*, int*> m_OutIntVariables;
		std::unordered_map<const char*, std::string*> m_OutStringVariables;

	private:

		friend class SystemRegistry;
		friend class CollisionListener2D;
		friend class ScriptingSystem;
		friend class WorldAdmin;

		/// RTTR

		RTTR_ENABLE()

		RTTR_REGISTRATION_FRIEND
	};
}