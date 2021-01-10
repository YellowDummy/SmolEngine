#include "stdafx.h"
#include "CollisionListener2D.h"
#include "ECS/Actor.h"
#include "ECS/WorldAdmin.h"
#include "ECS/Systems/ScriptingSystem.h"

#include "Scripting/BehaviourPrimitive.h"

namespace SmolEngine
{
	void CollisionListener2D::BeginContact(b2Contact* contact)
	{
		void* dataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* dataB = contact->GetFixtureB()->GetBody()->GetUserData();
		const auto actorB = static_cast<Actor*>(dataB);

		if (IsValid(actorB))
		{
			const auto actorA = static_cast<Actor*>(dataA);
			if (contact->GetFixtureA()->IsSensor())
			{
				ScriptingSystem::OnCollisionBegin(actorB, actorA, true);
				return;
			}

			ScriptingSystem::OnCollisionBegin(actorB, actorA, false);
		}
	}

	void CollisionListener2D::EndContact(b2Contact* contact)
	{
		void* dataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* dataB = contact->GetFixtureB()->GetBody()->GetUserData();

		const auto actorB = static_cast<Actor*>(dataB);

		if (IsValid(actorB))
		{
			const auto actorA = static_cast<Actor*>(dataA);
			if (contact->GetFixtureA()->IsSensor())
			{
				ScriptingSystem::OnCollisionEnd(actorB, actorA, true);
				return;
			}

			ScriptingSystem::OnCollisionEnd(actorB, actorA, false);
		}
	}

	void CollisionListener2D::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{

	}

	void CollisionListener2D::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	{

	}

	bool CollisionListener2D::IsValid(Actor* actor) const
	{
		return actor != nullptr;
	}
}