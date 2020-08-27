#pragma once

#include "Core/Core.h"
#include "box2d/box2d.h"

namespace SmolEngine
{
	class Actor;

	enum class BodyType: uint16_t
	{
		Static, Kinematic, Dynamic
	};

	class Rigidbody2D
	{
	public:
		int m_Type = (int)BodyType::Static;

		float m_GravityScale = 1.0f;
		float m_Friction = 0.3f;
		float m_Restitution = 0.0f;
		float m_Density = 1.0f;
		float m_ShapeX = 1.0f;
		float m_ShapeY = 1.0f;
		bool m_canSleep = true;
		bool m_IsAwake = true;
		bool m_IsBullet = false;

		Rigidbody2D(Ref<Actor> actor, b2World* world, BodyType type);
		~Rigidbody2D() = default;

		void CreateBody();
		void DeleteBody();

		b2Body* GetBody() const { return m_Body; }

	private:
		b2BodyType FindType(uint16_t type);

	private:
		Ref<Actor> m_Actor;
		b2World* m_World;
		b2Body* m_Body = nullptr;
		b2Fixture* m_Fixture = nullptr;
	};
}