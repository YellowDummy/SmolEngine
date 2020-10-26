#include "stdafx.h"
#include "Actor.h"
#include "Core/ECS/Scene.h"


#include "Core/ECS/ComponentTuples/DefaultBaseTuple.h"
#include "Core/ECS/ComponentTuples/CameraBaseTuple.h"
#include "Core/ECS/ComponentTuples/PhysicsBaseTuple.h"
#include "Core/ECS/ComponentTuples/ResourceTuple.h"


namespace SmolEngine
{
	Actor::Actor()
		: 
		Parent(nullptr)
	{

	}

	Actor::Actor(const ActorBaseType baseType, entt::entity entity, size_t index)
		:

		Parent(nullptr),

		ActorType(baseType),

		Entity(entity),

		Index(index)
	{

	}


	Ref<Actor> Actor::GetChildByName(const std::string& name)
	{
		if (Childs.empty()) { return nullptr; }
		auto set = Scene::GetScene()->GetIDSet();
		size_t id = set[name];

		return nullptr;
	}

	Ref<Actor> Actor::GetChildByTag(const std::string& tag)
	{

		return nullptr;
	}

	std::string& Actor::GetName() const
	{
		return GetInfo()->Name;
	}

	std::string& Actor::GetTag() const
	{
		return GetInfo()->Tag;
	}

	const size_t Actor::GetID() const
	{
		return GetInfo()->ID;
	}

	DefaultBaseTuple* Actor::GetDefaultBaseTuple() const
	{
		return Scene::GetScene()->GetTuple<DefaultBaseTuple>(Entity);
	}

	PhysicsBaseTuple* Actor::GetPhysicsBaseTuple() const
	{
		return Scene::GetScene()->GetTuple<PhysicsBaseTuple>(Entity);
	}

	CameraBaseTuple* Actor::GetCameraBaseTuple() const
	{
		return Scene::GetScene()->GetTuple<CameraBaseTuple>(Entity);
	}

	HeadComponent* Actor::GetInfo() const
	{
		switch (ActorType)
		{
		case ActorBaseType::DefaultBase:
		{
			DefaultBaseTuple* defTuple = GetDefaultBaseTuple();
			return &defTuple->Info;
		}
		case ActorBaseType::PhysicsBase:
		{
			PhysicsBaseTuple* phTuple = GetPhysicsBaseTuple();
			return &phTuple->Info;
		}
		case ActorBaseType::CameraBase:
		{
			CameraBaseTuple* camTuple = GetCameraBaseTuple();
			return &camTuple->Info;
		}
		default:
			return nullptr;
		}
	}
}