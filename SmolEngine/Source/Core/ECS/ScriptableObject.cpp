#include "stdafx.h"
#include "Core/ECS/ScriptableObject.h"
#include "Core/ECS/Actor.h"
#include "Core/ECS/Scene.h"

namespace SmolEngine
{
	ScriptableObject::ScriptableObject()
	{

	}

	Ref<Actor> ScriptableObject::GetParent()
	{
		return m_Actor->GetParent();
	}

	void ScriptableObject::SetParent(Ref<Actor> parent)
	{
		m_Actor->SetParent(parent);
	}

	std::vector<Ref<Actor>>& ScriptableObject::GetChilds()
	{
		return m_Actor->GetChilds();
	}

	std::vector<Ref<Actor>> ScriptableObject::GetActorList()
	{
		return Scene::GetScene()->GetActorList();
	}

	std::vector<Ref<Actor>> ScriptableObject::GetActorListByTag(const std::string& tag)
	{
		return Scene::GetScene()->GetActorListByTag(tag);
	}

	void ScriptableObject::AddChild(Ref<Actor> child)
	{
		Scene::GetScene()->AddChild(m_Actor, child);
	}

	void ScriptableObject::RemoveChild(Ref<Actor> child)
	{
		Scene::GetScene()->RemoveChild(m_Actor, child);
	}

	Ref<Actor> ScriptableObject::FindChildByName(const std::string& name)
	{
		return m_Actor->GetChildByName(name);
	}

	Ref<Actor> ScriptableObject::FindChildByTag(const std::string& tag)
	{
		return m_Actor->GetChildByTag(tag);
	}

	Ref<Actor> ScriptableObject::FindActorByName(const std::string& name)
	{
		return nullptr;
	}

	Ref<Actor> ScriptableObject::FindActorByTag(const std::string& tag)
	{
		return nullptr;
	}

	void ScriptableObject::PushOutVariable(const char* keyName, std::any val, OutValueType type)
	{
		switch (type)
		{
		case SmolEngine::OutValueType::Float:
		{
			auto result = m_OutFloatVariables.find(keyName);
			if (result == m_OutFloatVariables.end())
			{
				m_OutFloatVariables[keyName] = std::any_cast<float*>(val);
			}

			break;
		}
		case SmolEngine::OutValueType::Int:
		{
			auto result = m_OutIntVariables.find(keyName);
			if (result == m_OutIntVariables.end())
			{
				m_OutIntVariables[keyName] = std::any_cast<int*>(val);
			}

			break;
		}
		case SmolEngine::OutValueType::String:
		{
			auto result = m_OutStringVariables.find(keyName);
			if (result == m_OutStringVariables.end())
			{
				m_OutStringVariables[keyName] = std::any_cast<std::string*>(val);
			}

			break;
		}
		default:

			break;
		}
	}
}