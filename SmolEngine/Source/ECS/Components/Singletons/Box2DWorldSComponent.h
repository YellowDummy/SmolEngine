#pragma once

#include "Physics2D/Box2D/CollisionListener2D.h"
#include "Physics2D/Box2D/CollisionFilter2D.h"

namespace SmolEngine
{
	// Note:
    // S - Singleton Component
    // Contains Box2D world and collision listener / filter

	struct Box2DWorldSComponent
	{
		Box2DWorldSComponent();

		Box2DWorldSComponent(const float gravityX, const float gravityY);

		~Box2DWorldSComponent();

		/// Data

		b2World World = b2World({ 0.0f, -9.81f });
		CollisionListener2D m_CollisionListener2D = {};
		CollisionFilter2D m_CollisionFilter2D = {};

		/// Getters

		static Box2DWorldSComponent* Get() { return Instance; }

	private:

		static Box2DWorldSComponent* Instance;
	};
}