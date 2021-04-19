#include "stdafx.h"
#include "ECS/Actor.h"
#include "ECS/WorldAdmin.h"
#include "ECS/ComponentsCore.h"

namespace SmolEngine
{
	Actor::Actor()
		: 
		m_Parent(nullptr) {}

	Actor::Actor(entt::entity entity, size_t index)
		:
		m_Parent(nullptr),
		m_Entity(entity),
		m_Index(index) {}

	std::vector<Actor*>& Actor::GetChilds()
	{
		return m_Childs;
	}

	Actor* Actor::GetChildByName(const std::string& name)
	{
		if (m_Childs.empty()) { return nullptr; }
		auto& set = WorldAdmin::GetSingleton()->GetActiveScene()->GetIDSet();
		size_t id = set[name];

		return nullptr;
	}

	Actor* Actor::GetChildByTag(const std::string& tag)
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

	const uint32_t Actor::GetID() const
	{
		return GetInfo()->ID;
	}

	const size_t Actor::GetComponentsCount() const
	{
		return m_ComponentsCount;
	}

	void Actor::SetParent(Actor* parent)
	{
	}

	const HeadComponent* Actor::GetInfo() const
	{

		return WorldAdmin::GetSingleton()->GetActiveScene()->GetComponent<HeadComponent>(m_Entity);
	}
}