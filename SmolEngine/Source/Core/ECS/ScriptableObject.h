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

#define OUT_FLOAT(name, value) PushOutVariable(name, value, OutValueType::Float)
#define OUT_INT(name, value) PushOutVariable(name, value, OutValueType::Int)
#define OUT_STRING(name, value) PushOutVariable(name, value, OutValueType::String)

		bool Enabled = true;

		ScriptableObject();
		ScriptableObject(Ref<Actor> actor)
			: m_Actor(actor)
		{
		}

		virtual ~ScriptableObject() {}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) { return m_Actor->AddComponent<T>(args); }

		template<typename T>
		T& GetComponent() { return m_Actor->GetComponent<T>(); }

		template<typename T>
		bool DeleteComponent() { return m_Actor->DeleteComponent<T>(); }

		template<typename T>
		bool HasComponent() { return m_Actor->HasComponent<T>(); }

		std::string& GetName();
		const std::string& GetTag();

		Ref<Actor> GetParent();
		void SetParent(Ref<Actor> parent);

		std::vector<Ref<Actor>>& GetChilds();
		std::vector<Ref<Actor>>  GetActorList();
		std::vector<Ref<Actor>>  GetActorListByTag(const std::string& tag);

		void AddChild(Ref<Actor> child);
		void RemoveChild(Ref<Actor> child);

		Ref<Actor> FindChildByName(const std::string& name);
		Ref<Actor> FindChildByTag(const std::string& tag);
		Ref<Actor> FindActorByName(const std::string& name);
		Ref<Actor> FindActorByTag(const std::string& tag);
		Ref<Actor> GetActor() { return m_Actor; }

		virtual void Start() {  };
		virtual void OnUpdate(DeltaTime deltaTime) {};
		virtual void OnDestroy() {};
		virtual void OnValidate() {}

		//Must be implemented by the user in order to register an external script in the engine
		virtual std::shared_ptr<ScriptableObject> Instantiate() { NATIVE_ERROR("ScriptableObject: No matching overloaded function found"); return nullptr; };

		void PushOutVariable(const char* keyName, std::any val, OutValueType type);

	private:
		friend class cereal::access;
		friend class Scene;
		friend class EditorLayer;

		std::unordered_map<const char*, float*> m_OutFloatVariables;
		std::unordered_map<const char*, int*> m_OutIntVariables;
		std::unordered_map<const char*, std::string*> m_OutStringVariables;

		size_t ActorID = 0;

		Ref<Actor> m_Actor;

	};

}