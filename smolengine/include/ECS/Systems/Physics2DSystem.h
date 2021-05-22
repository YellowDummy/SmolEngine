#pragma once
#include "Core/Core.h"
#include "ECS/Components/Singletons/Box2DWorldSComponent.h"

#ifndef FROSTIUM_SMOLENGINE_IMPL
#define FROSTIUM_SMOLENGINE_IMPL
#endif
#include <Frostium3D/Common/Time.h>

#include <box2d/box2d.h>
#include <vector>

namespace SmolEngine
{
	struct Box2DWorldSComponent;
	struct TransformComponent;
	struct PhysicsBaseTuple;
	struct Rigidbody2DComponent;
	struct RayCast2DHitInfo;
	struct DistanceJointInfo;
	struct RevoluteJointInfo;
	struct PrismaticJointInfo;
	struct RopeJointInfo;
	struct JointInfo;

	class Body2D;
	enum class JointType: uint16_t;

	class Physics2DSystem
	{
	public:

		Physics2DSystem() = default;

		// Forces
		static void AddForce(Rigidbody2DComponent* body, const glm::vec2& force, bool wakeBody = true);
		static void AddForce(Rigidbody2DComponent* body, const glm::vec2& force, const glm::vec2& point, bool wakeBody = true);
		// RayCasting
		static void RayCast(const glm::vec2& startPoisition, const glm::vec2& targerPosition, RayCast2DHitInfo& hitInfo);
		static void CircleCast(const glm::vec2& startPoisition, const float distance, std::vector<RayCast2DHitInfo>& outHits);

	private:

		static void OnBeginWorld();
		static void OnUpdate(float delta);
		static void OnEndWorld();

		// Body Factory
		static void CreateBody(Rigidbody2DComponent* body, TransformComponent* tranform, Actor* actor);

		// Joint Factory
		static const bool BindJoint(Rigidbody2DComponent* bodyA, Rigidbody2DComponent* bodyB, JointType type, JointInfo* info, b2World* world);
		static const bool DeleteJoint(Rigidbody2DComponent* body, b2World* word);

	private:

		// Body Types
		static void CreateStatic(Body2D* bodyDef);
		static void CreateKinematic(Body2D* bodyDef);
		static void CreateDynamic(Body2D* bodyDef);
		// Joint Types
		static bool CreateDistanceJoint(Body2D* bodyA, Body2D* bodyB, DistanceJointInfo* info, b2World* world);
		static bool CreateRevoluteJoint(Body2D* bodyA, Body2D* bodyB, RevoluteJointInfo* info, b2World* world);
		static bool CreatePrismaticJoint(Body2D* bodyA, Body2D* bodyB, PrismaticJointInfo* info, b2World* world);
		static bool CreateRopeJoint(Body2D* bodyA, Body2D* bodyB, RopeJointInfo* info, b2World* world);
		// Helpers
		static void UpdateTransforms();
		static b2BodyType FindType(uint16_t type);

	private:

		inline static WorldAdminStateSComponent*  m_World = nullptr;
		inline static Box2DWorldSComponent*       m_State = nullptr;

		friend class EditorLayer;
		friend class WorldAdmin;
	};
}