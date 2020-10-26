#include "stdafx.h"
#include "Box2DPhysicsSystem.h"

#include "Core/ECS/ComponentTuples/PhysicsBaseTuple.h"
#include "Core/ECS/Components/Singletons/Box2DWorldSComponent.h"
#include "Core/Physics2D/Box2D/RayCast2D.h"
#include "Core/SLog.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

namespace SmolEngine
{
	void Box2DPhysicsSystem::OnPlay(Box2DWorldSComponent* data)
	{
		//Setting Box2D Filtering

		data->World->SetContactFilter(data->m_CollisionFilter2D);

		//Setting Box2D Collision Callbacks

		data->World->SetContactListener(data->m_CollisionListener2D);
	}

	void Box2DPhysicsSystem::OnUpdate(DeltaTime delta, uint32_t velocityIterations, uint32_t positionIterations, Box2DWorldSComponent* data)
	{
		data->World->Step(delta, velocityIterations, positionIterations);
	}

	void Box2DPhysicsSystem::CreateBody(PhysicsBaseTuple& tuple, b2World* world)
	{
		auto& body = tuple.Body.Body;
		{
			b2BodyDef bodyDef;
			bodyDef.type = FindType(body.m_Type);
			bodyDef.gravityScale = body.m_GravityScale;
			bodyDef.bullet = body.m_IsBullet;
			bodyDef.awake = body.m_IsAwake;
			bodyDef.allowSleep = body.m_canSleep;
			bodyDef.angle = tuple.Transform.Rotation.x;
			//bodyDef.userData = actor.get();

			bodyDef.position.Set(tuple.Transform.WorldPos.x, tuple.Transform.WorldPos.y);
			body.m_Body = world->CreateBody(&bodyDef);
		}

		if (body.m_Density == 1.0f)
		{
			if (body.m_ShapeType == (int)ShapeType::Box)
			{
				body.m_Density = body.m_Mass / ((body.m_Shape.x * 2.0f) * (body.m_Shape.y * 2.0f));
			}
			if (body.m_ShapeType == (int)ShapeType::Cirlce)
			{
				body.m_Density = body.m_Mass / ((body.m_Radius * 2.0f) * (body.m_Radius));
			}
		}

		switch (body.m_Type)
		{

		case (int)Body2DType::Static: CreateStatic(&body); break;

		case (int)Body2DType::Kinematic: CreateKinematic(&body); break;

		case (int)Body2DType::Dynamic: CreateDynamic(&body); break;

		default: break;
		}
	}

	void Box2DPhysicsSystem::DeleteBody(Body2DComponent* body, b2World* world)
	{
		world->DestroyBody(body->Body.m_Body);
	}

	const bool Box2DPhysicsSystem::BindJoint(Body2DComponent* bodyA, Body2DComponent* bodyB, JointType type, JointInfo* info, b2World* world)
	{
		auto& bodyRefA = bodyA->Body;
		auto& bodyRefB = bodyB->Body;

		if (bodyRefA.m_Joint)
		{
			world->DestroyJoint(bodyRefA.m_Joint);
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

			return CreateDistanceJoint(&bodyRefA, &bodyRefB, distanceInfo, world);
		}
		case SmolEngine::JointType::Revolute:
		{
			const auto revoluteInfo = static_cast<RevoluteJointInfo*>(info);
			if (!revoluteInfo)
			{
				NATIVE_ERROR("Joint: invalid info!");
				return false;
			}

			return CreateRevoluteJoint(&bodyRefA, &bodyRefB, revoluteInfo, world);
		}
		case SmolEngine::JointType::Prismatic:
		{
			const auto prismaticInfo = static_cast<PrismaticJointInfo*>(info);
			if (!prismaticInfo)
			{
				NATIVE_ERROR("Joint: invalid info!");
				return false;
			}

			return CreatePrismaticJoint(&bodyRefA, &bodyRefB, prismaticInfo, world);
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

			return CreateRopeJoint(&bodyRefA, &bodyRefB, ropeInfo, world);
		}
		default:
			break;
		}

		return true;
	}

	const bool Box2DPhysicsSystem::DeleteJoint(Body2DComponent* body, b2World* world)
	{
		world->DestroyJoint(body->Body.m_Joint);
		return true;
	}

	const RayCast2DHitInfo& Box2DPhysicsSystem::RayCast(const glm::vec2& startPoisition, const glm::vec2& targerPosition, b2World* world)
	{
		RayCast2D ray;
		world->RayCast(&ray, { startPoisition.x, startPoisition.y }, { targerPosition.x, targerPosition.y });
		return ray.GetInfo();
	}

	const std::vector<RayCast2DHitInfo> Box2DPhysicsSystem::CircleCast(const glm::vec2& startPoisition, float distance, b2World* world)
	{
		std::vector<RayCast2DHitInfo> infoList;
		std::vector<size_t> idList;

		glm::vec2 output = glm::vec2(1.0f);
		output = glm::normalize(output);

		for (float r = 0; r < 360; ++r)
		{
			output = glm::rotate(output, glm::radians(r));
			output *= distance;
			output += startPoisition;

			RayCast2D ray;
			world->RayCast(&ray, { startPoisition.x, startPoisition.y }, { output.x, output.y });
			const auto info = ray.GetInfo();
			if (info.IsBodyHitted)
			{
				//size_t tempID = info.Actor->GetID();
				size_t tempID = 0;

				if (std::find(idList.begin(), idList.end(), tempID) == idList.end())
				{
					idList.push_back(tempID);
					infoList.push_back(info);
				}
			}
		}

		return infoList;
	}

	b2BodyType Box2DPhysicsSystem::FindType(uint16_t type)
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

	void Box2DPhysicsSystem::CreateStatic(Body2D* bodyDef)
	{
		switch (bodyDef->m_ShapeType)
		{
		case (int)ShapeType::Box:
		{
			b2PolygonShape box;
			box.SetAsBox(bodyDef->m_Shape.x, bodyDef->m_Shape.y);
			bodyDef->m_Fixture = bodyDef->m_Body->CreateFixture(&box, 0.0f);
			break;
		}
		case (int)ShapeType::Cirlce:
		{
			b2CircleShape circle;
			circle.m_radius = bodyDef->m_Radius;
			circle.m_p.x = bodyDef->m_Offset.x;
			circle.m_p.y = bodyDef->m_Offset.y;
			bodyDef->m_Fixture = bodyDef->m_Body->CreateFixture(&circle, 0.0f);
			break;
		}
		default:
			break;
		}

		b2Filter filter;
		filter.groupIndex = bodyDef->m_CollisionLayer;
		bodyDef->m_Fixture->SetFilterData(filter);
		bodyDef->m_Fixture->SetSensor(bodyDef->m_IsTrigger);
	}

	void Box2DPhysicsSystem::CreateKinematic(Body2D* bodyDef)
	{
		b2MassData mass;
		mass.center = { bodyDef->m_MassCenter.x,  bodyDef->m_MassCenter.y };
		mass.I = bodyDef->m_InertiaMoment;
		mass.mass = bodyDef->m_Mass;

		switch (bodyDef->m_ShapeType)
		{
		case (int)ShapeType::Box:
		{
			b2PolygonShape box;
			box.SetAsBox(bodyDef->m_Shape.x, bodyDef->m_Shape.y);
			box.ComputeMass(&mass, bodyDef->m_Density);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &box;
			fixtureDef.density = bodyDef->m_Density;
			fixtureDef.friction = bodyDef->m_Density;
			fixtureDef.restitution = bodyDef->m_Restitution;
			bodyDef->m_Fixture = bodyDef->m_Body->CreateFixture(&fixtureDef);
			break;
		}
		case (int)ShapeType::Cirlce:
		{
			b2CircleShape circle;
			circle.m_radius = bodyDef->m_Radius;
			circle.m_p.x = bodyDef->m_Offset.x;
			circle.m_p.y = bodyDef->m_Offset.y;
			circle.ComputeMass(&mass, bodyDef->m_Density);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circle;
			fixtureDef.density = bodyDef->m_Density;
			fixtureDef.friction = bodyDef->m_Density;
			fixtureDef.restitution = bodyDef->m_Restitution;
			bodyDef->m_Fixture = bodyDef->m_Body->CreateFixture(&fixtureDef);
			break;
		}
		default:
			break;
		}

		b2Filter filter;
		filter.groupIndex = bodyDef->m_CollisionLayer;
		bodyDef->m_Fixture->SetFilterData(filter);
		bodyDef->m_Fixture->SetSensor(bodyDef->m_IsTrigger);
	}

	void Box2DPhysicsSystem::CreateDynamic(Body2D* bodyDef)
	{
		b2MassData mass;
		mass.center = { bodyDef->m_MassCenter.x,  bodyDef->m_MassCenter.y };
		mass.I = bodyDef->m_InertiaMoment;
		mass.mass = bodyDef->m_Mass;

		switch (bodyDef->m_ShapeType)
		{
		case (int)ShapeType::Box:
		{
			b2PolygonShape box;
			box.SetAsBox(bodyDef->m_Shape.x, bodyDef->m_Shape.y);
			box.ComputeMass(&mass, bodyDef->m_Density);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &box;
			fixtureDef.density = bodyDef->m_Density;
			fixtureDef.friction = bodyDef->m_Density;
			fixtureDef.restitution = bodyDef->m_Restitution;
			bodyDef->m_Fixture = bodyDef->m_Body->CreateFixture(&fixtureDef);
			break;
		}
		case (int)ShapeType::Cirlce:
		{
			b2CircleShape circle;
			circle.m_radius = bodyDef->m_Radius;
			circle.m_p.x = bodyDef->m_Offset.x;
			circle.m_p.y = bodyDef->m_Offset.y;
			circle.ComputeMass(&mass, bodyDef->m_Density);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circle;
			fixtureDef.density = bodyDef->m_Density;
			fixtureDef.friction = bodyDef->m_Density;
			fixtureDef.restitution = bodyDef->m_Restitution;
			bodyDef->m_Fixture = bodyDef->m_Body->CreateFixture(&fixtureDef);
			break;
		}
		default:
			break;
		}

		b2Filter filter;
		filter.groupIndex = bodyDef->m_CollisionLayer;
		bodyDef->m_Fixture->SetFilterData(filter);
		bodyDef->m_Fixture->SetSensor(bodyDef->m_IsTrigger);
	}

	bool Box2DPhysicsSystem::CreateDistanceJoint(Body2D* bodyA, Body2D* bodyB, DistanceJointInfo* info, b2World* world)
	{
		const auto& BodyB = bodyB->m_Body;
		const auto& BodyA = bodyA->m_Body;

		b2DistanceJointDef jointDef;
		jointDef.Initialize(BodyA, BodyB, BodyA->GetPosition(), BodyA->GetPosition());
		{
			jointDef.collideConnected = info->CollideConnected;
			jointDef.damping = info->Damping;
			jointDef.length = info->Length;
			jointDef.stiffness = info->Stiffness;

			jointDef.localAnchorA = { info->LocalAnchorA.x, info->LocalAnchorA.y };
			jointDef.localAnchorB = { info->LocalAnchorB.x, info->LocalAnchorB.y };
		}

		bodyA->m_Joint = world->CreateJoint(&jointDef);
		return true;
	}

	bool Box2DPhysicsSystem::CreateRevoluteJoint(Body2D* bodyA, Body2D* bodyB, RevoluteJointInfo* info, b2World* world)
	{
		const auto& BodyB = bodyB->m_Body;
		const auto& BodyA = bodyA->m_Body;

		b2RevoluteJointDef jointDef;

		jointDef.Initialize(BodyA, BodyB, BodyA->GetWorldCenter());
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

		bodyA->m_Joint = world->CreateJoint(&jointDef);
		return true;
	}

	bool Box2DPhysicsSystem::CreatePrismaticJoint(Body2D* bodyA, Body2D* bodyB, PrismaticJointInfo* info, b2World* world)
	{
		const auto& BodyB = bodyB->m_Body;
		const auto& BodyA = bodyA->m_Body;

		b2PrismaticJointDef jointDef;
		b2Vec2 worldAxis(1.0f, 0.0f);
		jointDef.Initialize(BodyA, BodyB, BodyA->GetWorldCenter(), worldAxis);
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

		bodyA->m_Joint = world->CreateJoint(&jointDef);
		return true;
	}

	bool Box2DPhysicsSystem::CreateRopeJoint(Body2D* bodyA, Body2D* bodyB, RopeJointInfo* info, b2World* world)
	{
		const auto& BodyB = bodyB->m_Body;
		const auto& BodyA = bodyA->m_Body;

		b2RopeJointDef jointDef;
		{
			jointDef.collideConnected = info->CollideConnected;
			jointDef.bodyA = BodyA;
			jointDef.bodyB = BodyB;
			jointDef.maxLength = info->MaxLength;

			jointDef.localAnchorA = { info->LocalAnchorA.x, info->LocalAnchorA.y };
			jointDef.localAnchorB = { info->LocalAnchorB.x, info->LocalAnchorB.y };
		}

		bodyA->m_Joint = world->CreateJoint(&jointDef);
		return true;
	}

	void Box2DPhysicsSystem::SetTransfrom(PhysicsBaseTuple& tuple)
	{
		auto& b2Pos = tuple.Body.Body.m_Body->GetTransform();

		tuple.Transform.WorldPos = { b2Pos.p.x, b2Pos.p.y, tuple.Transform.WorldPos.z };
		tuple.Transform.Rotation = { b2Pos.q.GetAngle(), tuple.Transform.Rotation.y, tuple.Transform.Rotation.z };
	}
}