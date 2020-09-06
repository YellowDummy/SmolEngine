#pragma once

#include "Core/Core.h"
#include "box2d/box2d.h"

#include <cereal/cereal.hpp>
#include <glm/glm.hpp>

namespace SmolEngine
{
	class Actor;

	enum class BodyType: uint16_t
	{
		Static, Kinematic, Dynamic
	};

	enum class ShapeType : uint16_t
	{
		Box, Cirlce
	};

	class Rigidbody2D
	{
	public:
		int m_Type = (int)BodyType::Static;
		int m_ShapeType = (int)ShapeType::Box;

		float m_GravityScale = 1.0f;
		float m_Friction = 0.3f;
		float m_Restitution = 0.0f;
		float m_Density = 1.0f;
		float m_Radius = 1.0f;

		bool m_canSleep = true;
		bool m_IsAwake = true;
		bool m_IsBullet = false;

		glm::vec2 m_Shape = glm::vec2(1.0f);
		glm::vec2 m_Offset = glm::vec2(0.0f);

	public:
		Rigidbody2D() = default;
		Rigidbody2D(Ref<Actor> actor, BodyType type);
		Rigidbody2D(Ref<Actor> actor, int type);
		~Rigidbody2D() = default;

		void CreateBody(b2World* world);
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
			archive(m_canSleep, m_Actor, m_Density, m_Friction, m_GravityScale, m_IsAwake, m_IsBullet, m_Restitution, m_Shape.x, m_Shape.y, m_Type, m_ShapeType, m_Radius, m_Offset.x, m_Offset.y);
			archive.serializeDeferments();
		}
	};
}