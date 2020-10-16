#pragma once

#include "Core/Core.h"
#include "Core/Physics2D/PhysicsEngine.h"
#include "Core/Physics2D/Box2D/CollisionListener2D.h"
#include "Core/Physics2D/Box2D/CollisionFilter2D.h"

#include <box2d/box2d.h>
#include <glm/glm.hpp>

namespace SmolEngine
{
	class Box2DEngine: public PhysicsEngine
	{
	public:

		Box2DEngine() = default;

		/// Main

		void Init(const glm::vec2& gravity = glm::vec2(0.0f, -8.91f)) override;

		void OnPlay() override;

		void OnEndPlay() override;

		void Reset() override;

		void Update(DeltaTime delta, uint32_t velocityIterations, uint32_t positionIterations) override;

		/// Getters

		void* GetWorld() override { return m_World; }

	private:

		glm::vec2& m_Gravity = glm::vec2(1.0f);

		b2World* m_World = nullptr;

		CollisionListener2D* m_CollisionListener2D = nullptr;

		CollisionFilter2D* m_CollisionFilter2D = nullptr;

	};
}