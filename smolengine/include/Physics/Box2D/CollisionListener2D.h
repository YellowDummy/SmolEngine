#pragma once
#include "Core/Core.h"

#include <box2d/box2d.h>
#include <box2d/b2_world_callbacks.h>

namespace SmolEngine
{
	class Actor;

	class CollisionListener2D : public b2ContactListener
	{
	public:

		// Contact
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;

		// Solve
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

	private:

		bool IsValid(Actor* actor) const;

	private:

		friend class WorldAdmin;
	};
}