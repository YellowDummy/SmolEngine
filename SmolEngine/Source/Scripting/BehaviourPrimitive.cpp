#include "stdafx.h"
#include "BehaviourPrimitive.h"

namespace SmolEngine
{
	const std::string& BehaviourPrimitive::GetName()
	{
		return m_Actor->GetName();
	}

	const std::string& BehaviourPrimitive::GetTag()
	{
		return m_Actor->GetTag();
	}

	const size_t BehaviourPrimitive::GetID()
	{
		return m_Actor->GetID();
	}

	Ref<Actor> BehaviourPrimitive::FindActorByName(const std::string& name)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene().FindActorByName(name);
	}

	Ref<Actor> BehaviourPrimitive::FindActorByTag(const std::string& tag)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene().FindActorByTag(tag);
	}

	Ref<Actor> BehaviourPrimitive::FindActorByID(uint32_t id)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene().FindActorByID(id);
	}

	void BehaviourPrimitive::GetActors(std::vector<Ref<Actor>>& outList)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene().GetActorList(outList);
	}

	void BehaviourPrimitive::GetActorsWithTag(const std::string& tag, std::vector<Ref<Actor>>& outList)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene().GetActorListByTag(tag, outList);
	}

	void BehaviourPrimitive::PushOutProperty(const char* keyName, std::any val, OutValueType type)
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