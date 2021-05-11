#pragma once

#include "Core/Core.h"
#include <box2d/b2_world_callbacks.h>

namespace SmolEngine
{
	class CollisionFilter2D: public b2ContactFilter
	{
	public:

		bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) override;
	};
}