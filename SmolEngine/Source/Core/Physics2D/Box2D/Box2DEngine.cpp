#include "stdafx.h"
#include "Box2DEngine.h"

namespace SmolEngine
{
	void Box2DEngine::Init(const glm::vec2& gravity)
	{
		if (m_World && m_CollisionFilter2D && m_CollisionListener2D)
		{
			delete m_World, m_CollisionFilter2D, m_CollisionListener2D;
		}

		m_CollisionListener2D = new CollisionListener2D();
		m_CollisionFilter2D = new CollisionFilter2D();
		m_World = new b2World(b2Vec2{ gravity.x, gravity.y });
		m_Gravity = gravity;
	}

	void Box2DEngine::OnPlay()
	{
		//Setting Box2D Filtering
		m_World->SetContactFilter(m_CollisionFilter2D);

		//Setting Box2D Collision Callbacks
		m_World->SetContactListener(m_CollisionListener2D);
	}

	void Box2DEngine::OnEndPlay()
	{
	}

	void Box2DEngine::Reset()
	{
		Init(m_Gravity);
	}

	void Box2DEngine::Update(DeltaTime delta, uint32_t velocityIterations, uint32_t positionIterations)
	{
		m_World->Step(delta, velocityIterations, positionIterations);
	}
}