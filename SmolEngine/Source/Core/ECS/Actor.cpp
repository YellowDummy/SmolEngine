#include "stdafx.h"
#include "Actor.h"
#include "Core/ECS/Scene.h"

namespace SmolEngine
{
	Actor::Actor()
		:Reg(Scene::GetScene()->GetRegistry())
	{

	}

	Actor::Actor(const entt::entity& entity, entt::registry& reg, const std::string& name, const std::string& tag, const size_t id, const size_t index)
		:
		ID(id),
		Index(index),
		Parent(nullptr),
		Entity(entity),
		Tag(tag),
		Reg(reg),
		Name(name),
		IsDisabled(false)
	{

	}


	Ref<Actor> Actor::GetChildByName(const std::string& name)
	{
		if (Childs.empty()) { return nullptr; }
		auto set = Scene::GetScene()->GetIDSet();
		size_t id = set[name];

		for (auto child : Childs)
		{
			if (child->GetID() == id)
			{
				return child;
			}
		}

		return nullptr;
	}

	Ref<Actor> Actor::GetChildByTag(const std::string& tag)
	{
		if (Childs.empty()) { return nullptr; }
		for (auto child : Childs)
		{
			if (child->GetTag() == tag)
			{
				return child;
			}
		}

		return nullptr;
	}
}