#pragma once

#include "Core/Physics2D/Box2D/CollisionListener2D.h"
#include "Core/Physics2D/Box2D/CollisionFilter2D.h"

namespace SmolEngine
{
	// Note:
    // S - Singleton Component
    // Contains Box2D world and collision listener / filter

	struct Box2DWorldSComponent
	{
		Box2DWorldSComponent(); // Uses Earth's gravity (0, -9.81)

		Box2DWorldSComponent(const float gravityX, const float gravityY);

		~Box2DWorldSComponent();

		/// Data

		b2World* World = nullptr;
		CollisionListener2D* m_CollisionListener2D = nullptr;
		CollisionFilter2D* m_CollisionFilter2D = nullptr;

		/// Getters

		static Box2DWorldSComponent* Get() { return Instance; }

	private:

		static Box2DWorldSComponent* Instance;
	};
}