#include "stdafx.h"
#include "Rigidbody2D.h"
#include "Core/ECS/Actor.h"

namespace SmolEngine
{
	Rigidbody2D::Rigidbody2D(Ref<Actor> actor, BodyType type)
		:m_Actor(actor), m_Type((int)type)
	{

	}

	Rigidbody2D::Rigidbody2D(Ref<Actor> actor, int type)
		:m_Actor(actor), m_Type(type)
	{
	}

	void Rigidbody2D::CreateBody(b2World* world)
	{
		m_World = world;

		auto& m_Transform = m_Actor->GetComponent<TransformComponent>();
		b2BodyDef bodyDef;
		bodyDef.type = FindType(m_Type);
		bodyDef.gravityScale = m_GravityScale;
		bodyDef.bullet = m_IsBullet;
		bodyDef.awake = m_IsAwake;
		bodyDef.allowSleep = m_canSleep;
		bodyDef.angle = m_Transform.Rotation;

		bodyDef.position.Set(m_Transform.WorldPos.x, m_Transform.WorldPos.y);
		m_Body = m_World->CreateBody(&bodyDef);

		m_Transform.B2Data = std::make_shared<B2Data>();
		m_Transform.B2Data->B2Pos = m_Body->GetTransformPtr();
		m_Transform.B2Data->B2Rotation = m_Body->GetAnglePtr();
		m_Transform.B2Data->B2Body = m_Body;

		if (m_ShapeType == (int)ShapeType::Box)
		{
			b2PolygonShape box;
			box.SetAsBox(m_Shape.x, m_Shape.y);

			switch (m_Type)
			{
			case (int)BodyType::Dynamic:
			{
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &box;
				fixtureDef.density = m_Density;
				fixtureDef.friction = m_Density;
				fixtureDef.restitution = m_Restitution;
				m_Fixture = m_Body->CreateFixture(&fixtureDef);
				break;
			}
			case (int)BodyType::Static:
			{
				m_Fixture = m_Body->CreateFixture(&box, 0.0f);
				break;
			}
			default:
			{
				break;
			}
			}
		}
		else if (m_ShapeType == (int)ShapeType::Cirlce)
		{
			b2CircleShape circle;
			circle.m_radius = m_Radius;
			circle.m_p.x = m_Offset.x;
			circle.m_p.y = m_Offset.y;

			switch (m_Type)
			{
			case (int)BodyType::Dynamic:
			{
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circle;
				fixtureDef.density = m_Density;
				fixtureDef.friction = m_Density;
				fixtureDef.restitution = m_Restitution;
				m_Fixture = m_Body->CreateFixture(&fixtureDef);
				break;
			}
			case (int)BodyType::Static:
			{
				m_Fixture = m_Body->CreateFixture(&circle, 0.0f);
				break;
			}
			default:
			{
				break;
			}
			}
		}


	}

	void Rigidbody2D::DeleteBody()
	{
		m_World->DestroyBody(m_Body);
	}

	b2BodyType Rigidbody2D::FindType(uint16_t type)
	{
		if (type == (uint16_t)b2BodyType::b2_dynamicBody)
		{
			return b2BodyType::b2_dynamicBody;
		}
		else if (type == (uint16_t)b2BodyType::b2_kinematicBody)
		{
			return b2BodyType::b2_kinematicBody;
		}
		else if (type == (uint16_t)b2BodyType::b2_staticBody)
		{
			return b2BodyType::b2_staticBody;
		}
	}
}