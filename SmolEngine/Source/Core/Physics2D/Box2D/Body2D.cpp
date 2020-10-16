#include "stdafx.h"
#include "Body2D.h"
#include "Core/ECS/Actor.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

namespace SmolEngine
{
	Body2D::Body2D(Body2DType type)
		:m_Type((int)type)
	{

	}

	Body2D::Body2D(int type)
		:m_Type(type)
	{

	}

	void Body2D::CreateBody(b2World* world, Ref<Actor> actor)
	{
		m_World = world;

		if (!actor)
		{
			NATIVE_ERROR("Body2D: invalid actor!");
			return;
		}

		auto& m_Transform = actor->GetComponent<TransformComponent>();
		{
			b2BodyDef bodyDef;
			bodyDef.type = FindType(m_Type);
			bodyDef.gravityScale = m_GravityScale;
			bodyDef.bullet = m_IsBullet;
			bodyDef.awake = m_IsAwake;
			bodyDef.allowSleep = m_canSleep;
			bodyDef.angle = m_Transform.Rotation;
			bodyDef.userData = actor.get();

			bodyDef.position.Set(m_Transform.WorldPos.x, m_Transform.WorldPos.y);
			m_Body = m_World->CreateBody(&bodyDef);
		}

		{
			m_Transform.B2Data = std::make_shared<B2Data>();
			m_Transform.B2Data->B2Pos = m_Body->GetTransformPtr();
			m_Transform.B2Data->B2Rotation = m_Body->GetAnglePtr();
			m_Transform.B2Data->B2Body = m_Body;
		}

		if (m_Density == 1.0f)
		{
			if (m_ShapeType == (int)ShapeType::Box)
			{
				m_Density = m_Mass / ((m_Shape.x * 2.0f) * (m_Shape.y * 2.0f));
			}
			if (m_ShapeType == (int)ShapeType::Cirlce)
			{
				m_Density = m_Mass / ((m_Radius * 2.0f) * (m_Radius));
			}
		}

		switch (m_Type)
		{

		case (int)Body2DType::Static: CreateStatic(); break;

		case (int)Body2DType::Kinematic: CreateKinematic(); break;
					  
		case (int)Body2DType::Dynamic: CreateDynamic(); break;

		default: break;
		}

	}

	void Body2D::DeleteBody()
	{
		m_World->DestroyBody(m_Body);
	}

	const RayCast2DHitInfo& Body2D::RayCast(const glm::vec2& startPoisition, const glm::vec2& targerPosition)
	{
		RayCast2D ray;
		m_World->RayCast(&ray, { startPoisition.x, startPoisition.y }, { targerPosition.x, targerPosition.y });
		return ray.GetInfo();
	}

	const std::vector<RayCast2DHitInfo> Body2D::CircleCast(const glm::vec2& startPoisition, float distance)
	{
		std::vector<RayCast2DHitInfo> infoList;
		std::vector<size_t> idList;

		for (float r = 0; r < 360; ++r)
		{
			glm::vec2 output = glm::vec2(1.0f);
			output = glm::normalize(output);

			output = glm::rotate(output, glm::radians(r));
			output *= distance;
			output += startPoisition;

			RayCast2D ray;
			m_World->RayCast(&ray, { startPoisition.x, startPoisition.y }, { output.x, output.y });
			const auto info = ray.GetInfo();
			if (info.IsBodyHitted)
			{
				size_t tempID = info.Actor->GetID();
				if (std::find(idList.begin(), idList.end(), tempID) == idList.end())
				{
					idList.push_back(tempID);
					infoList.push_back(info);
				}
			}
		}

		return infoList;
	}

	const bool Body2D::BindJoint(Body2D* body, JointType type, JointInfo* info)
	{
		if (!info || !m_World)
		{
			return false;
		}

		if (m_Joint)
		{
			m_World->DestroyJoint(m_Joint);
		}

		switch (type)
		{
		case SmolEngine::JointType::Distance:
		{
			const auto distanceInfo = static_cast<DistanceJointInfo*>(info);
			if (!distanceInfo)
			{
				NATIVE_ERROR("Joint: invalid info!");
				return false;
			}

			return CreateDistanceJoint(body, distanceInfo);
		}
		case SmolEngine::JointType::Revolute:
		{
			const auto revoluteInfo = static_cast<RevoluteJointInfo*>(info);
			if (!revoluteInfo)
			{
				NATIVE_ERROR("Joint: invalid info!");
				return false;
			}

			return CreateRevoluteJoint(body, revoluteInfo);
		}
		case SmolEngine::JointType::Prismatic:
		{
			const auto prismaticInfo = static_cast<PrismaticJointInfo*>(info);
			if (!prismaticInfo)
			{
				NATIVE_ERROR("Joint: invalid info!");
				return false;
			}

			return CreatePrismaticJoint(body, prismaticInfo);
		}
		case SmolEngine::JointType::Gear:
		{
			return false;

			break;
		}
		case SmolEngine::JointType::Motor:
		{
			return false;

			break;
		}
		case SmolEngine::JointType::Rope:
		{
			const auto ropeInfo = static_cast<RopeJointInfo*>(info);
			if (!ropeInfo)
			{
				NATIVE_ERROR("Joint: invalid info!");
				return false;
			}

			return CreateRopeJoint(body, ropeInfo);
		}
		default:
			break;
		}

		return true;
	}

	const bool Body2D::DeleteJoint()
	{
		m_World->DestroyJoint(m_Joint);
		return true;
	}

	b2BodyType Body2D::FindType(uint16_t type)
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

		return b2BodyType::b2_staticBody;
	}

	void Body2D::CreateStatic()
	{
		switch (m_ShapeType)
		{
		case (int)ShapeType::Box:
		{
			b2PolygonShape box;
			box.SetAsBox(m_Shape.x, m_Shape.y);
			m_Fixture = m_Body->CreateFixture(&box, 0.0f);
			break;
		}
		case (int)ShapeType::Cirlce:
		{
			b2CircleShape circle;
			circle.m_radius = m_Radius;
			circle.m_p.x = m_Offset.x;
			circle.m_p.y = m_Offset.y;
			m_Fixture = m_Body->CreateFixture(&circle, 0.0f);
			break;
		}
		default:
			break;
		}

		b2Filter filter;
		filter.groupIndex = m_CollisionLayer;
		m_Fixture->SetFilterData(filter);
		m_Fixture->SetSensor(m_IsTrigger);
	}

	void Body2D::CreateKinematic()
	{
		b2MassData mass;
		mass.center = { m_MassCenter.x, m_MassCenter.y };
		mass.I = m_InertiaMoment;
		mass.mass = m_Mass;

		switch (m_ShapeType)
		{
		case (int)ShapeType::Box:
		{
			b2PolygonShape box;
			box.SetAsBox(m_Shape.x, m_Shape.y);
			box.ComputeMass(&mass, m_Density);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &box;
			fixtureDef.density = m_Density;
			fixtureDef.friction = m_Density;
			fixtureDef.restitution = m_Restitution;
			m_Fixture = m_Body->CreateFixture(&fixtureDef);
			break;
		}
		case (int)ShapeType::Cirlce:
		{
			b2CircleShape circle;
			circle.m_radius = m_Radius;
			circle.m_p.x = m_Offset.x;
			circle.m_p.y = m_Offset.y;
			circle.ComputeMass(&mass, m_Density);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circle;
			fixtureDef.density = m_Density;
			fixtureDef.friction = m_Density;
			fixtureDef.restitution = m_Restitution;
			m_Fixture = m_Body->CreateFixture(&fixtureDef);
			break;
		}
		default:
			break;
		}

		b2Filter filter;
		filter.groupIndex = m_CollisionLayer;
		m_Fixture->SetFilterData(filter);
		m_Fixture->SetSensor(m_IsTrigger);
	}

	void Body2D::CreateDynamic()
	{
		b2MassData mass;
		mass.center = { m_MassCenter.x, m_MassCenter.y };
		mass.I = m_InertiaMoment;
		mass.mass = m_Mass;

		switch (m_ShapeType)
		{
		case (int)ShapeType::Box:
		{
			b2PolygonShape box;
			box.SetAsBox(m_Shape.x, m_Shape.y);
			box.ComputeMass(&mass, m_Density);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &box;
			fixtureDef.density = m_Density;
			fixtureDef.friction = m_Density;
			fixtureDef.restitution = m_Restitution;
			m_Fixture = m_Body->CreateFixture(&fixtureDef);
			break;
		}
		case (int)ShapeType::Cirlce:
		{
			b2CircleShape circle;
			circle.m_radius = m_Radius;
			circle.m_p.x = m_Offset.x;
			circle.m_p.y = m_Offset.y;
			circle.ComputeMass(&mass, m_Density);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circle;
			fixtureDef.density = m_Density;
			fixtureDef.friction = m_Density;
			fixtureDef.restitution = m_Restitution;
			m_Fixture = m_Body->CreateFixture(&fixtureDef);
			break;
		}
		default:
			break;
		}

		b2Filter filter;
		filter.groupIndex = m_CollisionLayer;
		m_Fixture->SetFilterData(filter);
		m_Fixture->SetSensor(m_IsTrigger);
	}

	bool Body2D::CreateDistanceJoint(Body2D* body, DistanceJointInfo* info)
	{
		const auto& bodyB = body->GetBody();

		b2DistanceJointDef jointDef;
		jointDef.Initialize(m_Body, bodyB, m_Body->GetPosition(), body->GetBody()->GetPosition());
		{
			jointDef.collideConnected = info->CollideConnected;
			jointDef.damping = info->Damping;
			jointDef.length = info->Length;
			jointDef.stiffness = info->Stiffness;

			jointDef.localAnchorA = { info->LocalAnchorA.x, info->LocalAnchorA.y };
			jointDef.localAnchorB = { info->LocalAnchorB.x, info->LocalAnchorB.y };
		}

		m_Joint = m_World->CreateJoint(&jointDef);
		return true;
	}

	bool Body2D::CreateRevoluteJoint(Body2D* body, RevoluteJointInfo* info)
	{
		const auto& bodyB = body->GetBody();

		b2RevoluteJointDef jointDef;

		jointDef.Initialize(m_Body, bodyB, m_Body->GetWorldCenter());
		{
			jointDef.collideConnected = info->CollideConnected;
			jointDef.lowerAngle = info->LowerAngle;
			jointDef.upperAngle = info->UpperAngle;
			jointDef.enableLimit = info->EnableLimit;
			jointDef.maxMotorTorque = info->MaxMotorTorque;
			jointDef.motorSpeed = info->MotorSpeed;
			jointDef.enableMotor = info->EnableMotor;
			jointDef.referenceAngle = info->ReferenceAngle;

			jointDef.localAnchorA = { info->LocalAnchorA.x, info->LocalAnchorA.y };
			jointDef.localAnchorB = { info->LocalAnchorB.x, info->LocalAnchorB.y };
		}

		m_Joint = m_World->CreateJoint(&jointDef);
		return true;
	}

	bool Body2D::CreatePrismaticJoint(Body2D* body, PrismaticJointInfo* info)
	{
		const auto& bodyB = body->GetBody();

		b2PrismaticJointDef jointDef;
		b2Vec2 worldAxis(1.0f, 0.0f);
		jointDef.Initialize(m_Body, bodyB, m_Body->GetWorldCenter(), worldAxis);
		{
			jointDef.collideConnected = info->CollideConnected;
			jointDef.lowerTranslation = info->LowerTranslation;
			jointDef.localAxisA = { info->LocalAxisA.x,info->LocalAxisA.y };
			jointDef.upperTranslation = info->UpperTranslation;
			jointDef.enableLimit = info->EnableLimit;
			jointDef.maxMotorForce = info->MaxMotorForce;
			jointDef.motorSpeed = info->MotorSpeed;
			jointDef.enableMotor = info->EnableMotor;

			jointDef.localAnchorA = { info->LocalAnchorA.x, info->LocalAnchorA.y };
			jointDef.localAnchorB = { info->LocalAnchorB.x, info->LocalAnchorB.y };
		}

		m_Joint = m_World->CreateJoint(&jointDef);
		return true;
	}

	bool Body2D::CreateRopeJoint(Body2D* body, RopeJointInfo* info)
	{
		const auto& bodyB = body->GetBody();

		b2RopeJointDef jointDef;
		{
			jointDef.collideConnected = info->CollideConnected;
			jointDef.bodyA = m_Body;
			jointDef.bodyB = bodyB;
			jointDef.maxLength = info->MaxLength;

			jointDef.localAnchorA = { info->LocalAnchorA.x, info->LocalAnchorA.y };
			jointDef.localAnchorB = { info->LocalAnchorB.x, info->LocalAnchorB.y };
		}

		m_Joint = m_World->CreateJoint(&jointDef);
		return true;
	}
}