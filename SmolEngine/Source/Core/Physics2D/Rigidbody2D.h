#pragma once

#include "Core/Core.h"
#include "box2d/box2d.h"

#include <cereal/cereal.hpp>

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

		Rigidbody2D() = default;
		Rigidbody2D(Ref<Actor> actor, b2World* world, BodyType type);
		~Rigidbody2D() = default;

		void CreateBody();
		void DeleteBody();

		b2Body* GetBody() const { return m_Body; }

	private:
		b2BodyType FindType(uint16_t type);

	private:
		friend class cereal::access;
		friend class Scene;

		Ref<Actor> m_Actor = nullptr;
		b2World* m_World = nullptr;
		b2Body* m_Body = nullptr;
		b2Fixture* m_Fixture = nullptr;

		template<typename Archive>
		void serialize(Archive& archive) 
		{
			archive(m_canSleep, m_Actor, m_Density, m_Friction, m_GravityScale, m_IsAwake, m_IsBullet, m_Restitution, m_ShapeX, m_ShapeY, m_Type);
			archive.serializeDeferments();
		}
	};
}