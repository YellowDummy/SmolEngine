#include "stdafx.h"
#include "Physics2D/Box2D/CollisionFilter2D.h"

#include <box2d\b2_fixture.h>

namespace SmolEngine
{
	bool CollisionFilter2D::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
	{
        const b2Filter& filterA = fixtureA->GetFilterData();
        const b2Filter& filterB = fixtureB->GetFilterData();

        if (filterA.groupIndex == filterB.groupIndex)
        {
            return true;
        }

        return false;
	}
}