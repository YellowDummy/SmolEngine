#include "stdafx.h"
#include "Physics2D/Box2D/RayCast2D.h"
#include "ECS/Actor.h"

#include <box2d/b2_fixture.h>

namespace SmolEngine
{
	float RayCast2D::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
	{

		const auto actor = static_cast<Actor*>(fixture->GetBody()->GetUserData());

		m_Info.Actor = actor;
		m_Info.HitPoint = { point.x, point.y };
		m_Info.Normal = { normal.x, normal.y };
		m_Info.Fraction = fraction;
		m_Info.IsBodyHitted = true;

		return fraction;
	}

	const RayCast2DHitInfo& RayCast2D::GetHitInfo()
	{
		return m_Info;
	}
}