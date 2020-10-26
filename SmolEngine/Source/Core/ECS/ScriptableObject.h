#pragma once

#include "Core/Core.h"
#include "Core/Time.h"
#include "Core/SLog.h"
#include "Core/Scripting/OutValues.h"

#include <unordered_map>
#include <any>
#include <glm/glm.hpp>
#include <cereal/types/polymorphic.hpp>

namespace SmolEngine
{
	class Actor;

	struct ScriptableObject 	//Base Class *Struct* For All Script Classes
	{
		ScriptableObject();

		ScriptableObject(Ref<Actor> actor)
			: m_Actor(actor) {}

		virtual ~ScriptableObject() {}

		/// Operations with Actors

		std::vector<Ref<Actor>>& GetChilds();

		std::vector<Ref<Actor>>  GetActorList();

		std::vector<Ref<Actor>>  GetActorListByTag(const std::string& tag);


		Ref<Actor> FindChildByName(const std::string& name);

		Ref<Actor> FindChildByTag(const std::string& tag);

		Ref<Actor> FindActorByName(const std::string& name);

		Ref<Actor> FindActorByTag(const std::string& tag);


		/// Childs / Parents

		void SetParent(Ref<Actor> parent);

		void AddChild(Ref<Actor> child);

		void RemoveChild(Ref<Actor> child);


		/// Getters

		Ref<Actor> GetParent();

		Ref<Actor> GetActor() { return m_Actor; }

		/// Callbacks

		virtual void OnBeginPlay() {  };

		virtual void OnUpdate(DeltaTime deltaTime) {};

		virtual void OnDestroy() {};

		virtual void OnCollisionContact(Actor* actor) {};

		virtual void OnCollisionExit(Actor* actor) {};

		virtual void OnTriggerContact(Actor* actor) {}

		virtual void OnTriggerExit(Actor* actor) {}

		//Note:
		//Must be implemented by the user in order to register an external script in the engine
		virtual std::shared_ptr<ScriptableObject> Instantiate() { NATIVE_ERROR("ScriptableObject: No matching overloaded function found"); return nullptr; };

		/// Exposable Values

		void PushOutVariable(const char* keyName, std::any val, OutValueType type);

#define OUT_FLOAT(name, value) PushOutVariable(name, value, OutValueType::Float)

#define OUT_INT(name, value) PushOutVariable(name, value, OutValueType::Int)

#define OUT_STRING(name, value) PushOutVariable(name, value, OutValueType::String)

		/// Templates

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) { return m_Actor->AddComponent<T>(std::forward<Args>(args)...); }

		template<typename T>
		T& GetComponent() { return m_Actor->GetComponent<T>(); }

		template<typename T>
		bool DeleteComponent() { return m_Actor->DeleteComponent<T>(); }

		template<typename T>
		bool HasComponent() { return m_Actor->HasComponent<T>(); }

	public:

		bool Enabled = true;

	private:

		friend class cereal::access;
		friend class Scene;
		friend class EditorLayer;

		///

		std::unordered_map<const char*, float*> m_OutFloatVariables;

		std::unordered_map<const char*, int*> m_OutIntVariables;

		std::unordered_map<const char*, std::string*> m_OutStringVariables;

		///

		size_t ActorID = 0;

		Ref<Actor> m_Actor;

	};

}