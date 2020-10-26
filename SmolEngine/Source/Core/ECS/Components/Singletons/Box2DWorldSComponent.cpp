#include "stdafx.h"
#include "Box2DWorldSComponent.h"

namespace SmolEngine
{
	Box2DWorldSComponent* Box2DWorldSComponent::Instance = nullptr;

	Box2DWorldSComponent::Box2DWorldSComponent()
	{
		World = new b2World({ 0.0f, -9.81f });

		m_CollisionListener2D = new CollisionListener2D();
		m_CollisionFilter2D = new CollisionFilter2D();

		Instance = this;
	}

	Box2DWorldSComponent::Box2DWorldSComponent(const float gravityX, const float gravityY)
	{
		World = new b2World({ gravityX, gravityY });

		m_CollisionListener2D = new CollisionListener2D();
		m_CollisionFilter2D = new CollisionFilter2D();

		Instance = this;
	}

	Box2DWorldSComponent::~Box2DWorldSComponent()
	{
		if (!Instance) { return; }

		if (m_CollisionListener2D != nullptr)
		{
			delete m_CollisionListener2D;
		}

		if (m_CollisionFilter2D != nullptr)
		{
			delete m_CollisionFilter2D;
		}

		if (World != nullptr)
		{
			delete World;
		}

		Instance = nullptr;
	}
}