#include "stdafx.h"
#include "PhysicsTupleBehaviour.h"

#include "Core/ECS/WorldAdmin.h"
#include "Core/ECS/Actor.h"
#include "Core/ECS/Systems/Box2DPhysicsSystem.h"

namespace SmolEngine
{

	PhysicsBaseTuple& PhysicsTupleBehaviour::GetPhysicsBaseTuple()
	{
		return *WorldAdmin::GetScene()->GetTuple<PhysicsBaseTuple>(*m_Actor);
	}

	ResourceTuple* PhysicsTupleBehaviour::GetResourceTuple()
	{
		return WorldAdmin::GetScene()->GetTuple<ResourceTuple>(*m_Actor);
	}

	void PhysicsTupleBehaviour::AddForce(const PhysicsBaseTuple& tuple, const glm::vec2& force) const
	{
		Box2DPhysicsSystem::AddForce(tuple, force);
	}

	void PhysicsTupleBehaviour::AddForce(const PhysicsBaseTuple& tuple, const glm::vec2& force, const glm::vec2& point) const
	{
		Box2DPhysicsSystem::AddForce(tuple, force, point);
	}

	void PhysicsTupleBehaviour::AddForce(const glm::vec2& force)
	{
		Box2DPhysicsSystem::AddForce(GetPhysicsBaseTuple(), force);
	}

	void PhysicsTupleBehaviour::AddForce(const glm::vec2& force, const glm::vec2& point)
	{
		Box2DPhysicsSystem::AddForce(GetPhysicsBaseTuple(), force, point);
	}
}