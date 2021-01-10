#include "stdafx.h"
#include "Actor.h"

#include "ECS/WorldAdmin.h"
#include "ECS/ComponentTuples/DefaultBaseTuple.h"
#include "ECS/ComponentTuples/CameraBaseTuple.h"


namespace SmolEngine
{
	Actor::Actor()
		: 
		m_Parent(nullptr)
	{

	}

	Actor::Actor(const ActorBaseType baseType, entt::entity entity, size_t index)
		:

		m_Parent(nullptr),
		m_ActorType(baseType),
		m_Entity(entity),
		m_Index(index)
	{

	}


	Ref<Actor> Actor::GetChildByName(const std::string& name)
	{
		if (m_Childs.empty()) { return nullptr; }
		auto& set = WorldAdmin::GetSingleton()->GetActiveScene().GetIDSet();
		size_t id = set[name];

		return nullptr;
	}

	Ref<Actor> Actor::GetChildByTag(const std::string& tag)
	{
		return nullptr;
	}

	const std::string& Actor::GetName() const
	{
		return GetInfo()->Name;
	}

	const std::string& Actor::GetTag() const
	{
		return GetInfo()->Tag;
	}

	const size_t Actor::GetID() const
	{
		return GetInfo()->ID;
	}

	const size_t Actor::GetComponentsCount() const
	{
		return m_ComponentsCount;
	}

	const HeadComponent* Actor::GetInfo() const
	{
		switch (m_ActorType)
		{
		case ActorBaseType::DefaultBase:
		{
			return &WorldAdmin::GetSingleton()->GetActiveScene().GetComponent<DefaultBaseTuple>(m_Entity)->GetInfo();
		}
		case ActorBaseType::CameraBase:
		{
			return &WorldAdmin::GetSingleton()->GetActiveScene().GetComponent<CameraBaseTuple>(m_Entity)->GetInfo();
		}
		default:
			return nullptr;
		}
	}
}