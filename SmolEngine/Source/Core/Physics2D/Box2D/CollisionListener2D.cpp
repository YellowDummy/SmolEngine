#include "stdafx.h"
#include "CollisionListener2D.h"
#include "Core/ECS/Actor.h"
#include "Core/ECS/WorldAdmin.h"

#include "Core/Scripting/BaseClasses/BehaviourPrimitive.h"

namespace SmolEngine
{
	void CollisionListener2D::BeginContact(b2Contact* contact)
	{
		void* dataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* dataB = contact->GetFixtureB()->GetBody()->GetUserData();

		const auto actorB = static_cast<Actor*>(dataB);

#if 0
		if (IsValid(actorB))
		{
			const auto actorA = static_cast<Actor*>(dataA);
			Ref<WorldAdmin> worldAdmin = WorldAdmin::GetScene();
			BehaviourComponent* behaviour = worldAdmin->GetTuple<BehaviourComponent>(*actorB);

			if (behaviour == nullptr)
			{
				NATIVE_ERROR("CollisionListener2D:: Actor dosent have behaviour component");
				return;
			}

			auto& sysMap = worldAdmin->GetSystemMap();
			auto& sysRef = sysMap[behaviour->SystemName];

			worldAdmin->PrepareSystem(*behaviour, sysRef);

			if (contact->GetFixtureA()->IsSensor())
			{
				sysRef.type.invoke("OnCollisionContact", sysRef.variant, { actorA, true });
				return;
			}

			sysRef.type.invoke("OnCollisionContact", sysRef.variant, { actorA, false });
		}
#endif
	}

	void CollisionListener2D::EndContact(b2Contact* contact)
	{
		void* dataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* dataB = contact->GetFixtureB()->GetBody()->GetUserData();

		const auto actorB = static_cast<Actor*>(dataB);

#if 0
		if (IsValid(actorB))
		{
			const auto actorA = static_cast<Actor*>(dataA);

			Ref<WorldAdmin> worldAdmin = WorldAdmin::GetScene();
			auto& sysMap = worldAdmin->GetSystemMap();
			BehaviourComponent* behaviour = worldAdmin->GetTuple<BehaviourComponent>(*actorB);

			auto& sysRef = sysMap[behaviour->SystemName];
			auto& primitive = sysRef.variant.get_wrapped_value_non_const<BehaviourPrimitive>();
			primitive.m_Actor = behaviour->Actor;

			if (contact->GetFixtureA()->IsSensor())
			{
				sysRef.type.invoke("OnCollisionExit", sysRef.variant, { actorA, true });
				return;
			}

			sysRef.type.invoke("OnCollisionExit", sysRef.variant, { actorA, false });
		}
#endif
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