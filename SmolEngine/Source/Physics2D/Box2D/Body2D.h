#pragma once

#include "Core/Core.h"
#include "Physics2D/Box2D/Body2DDefs.h"
#include "Physics2D/Box2D/RayCast2D.h"

#include <cereal/cereal.hpp>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <box2d/b2_world_callbacks.h>

namespace SmolEngine
{
	class Actor;

	class Body2D
	{
	public:

		Body2D() = default;

		Body2D(Body2DType type);

		Body2D(int type);

	public:

		glm::vec2 m_Shape = glm::vec2(1.0f);
		glm::vec2 m_Offset = glm::vec2(0.0f);
		glm::vec2 m_MassCenter = glm::vec2(0.0f);

		float m_GravityScale = 1.0f;
		float m_Friction = 0.3f;
		float m_Restitution = 0.0f;
		float m_Density = 1.0f;
		float m_Radius = 0.5f;
		float m_InertiaMoment = 0.0f;
		float m_Mass = 1.0f;

		int m_Type = (int)Body2DType::Static; // imgui support
		int m_ShapeType = (int)ShapeType::Box;
		int m_CollisionLayer = 0;

		bool m_canSleep = true;
		bool m_IsAwake = true;
		bool m_IsBullet = false;
		bool m_IsTrigger = false;

		b2Body* m_Body = nullptr;
		b2Fixture* m_Fixture = nullptr;
		b2Joint* m_Joint = nullptr;

	private:

		friend class cereal::access;

		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive) 
		{
			archive(m_canSleep, m_Density, m_Friction, m_CollisionLayer, m_InertiaMoment, m_Mass, m_GravityScale,
				m_IsAwake, m_IsBullet, m_IsTrigger, m_Restitution, m_Shape.x, m_Shape.y, m_Type, m_ShapeType, m_Radius, m_Offset.x, m_Offset.y,
				m_MassCenter.x, m_MassCenter.y);

			archive.serializeDeferments();
		}
	};
}