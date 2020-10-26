#pragma once
#include "Core/Core.h"
#include "Core/Time.h"

#include "Core/ECS/Components/Singletons/Box2DWorldSComponent.h"
#include "Core/Physics2D/Box2D/Body2DDefs.h"

#include <box2d/box2d.h>
#include <vector>


namespace SmolEngine
{
	struct PhysicsBaseTuple;

	struct Body2DComponent;

	struct RayCast2DHitInfo;

	struct Box2DWorldSComponent;

	class Body2D;

	///

	class Box2DPhysicsSystem
	{
	public:

		Box2DPhysicsSystem() = default;

		///

	private:

		static void OnPlay(Box2DWorldSComponent* data);

		static void OnUpdate(DeltaTime delta, uint32_t velocityIterations, uint32_t positionIterations, Box2DWorldSComponent* data);

		/// Body Factory

		static void CreateBody(PhysicsBaseTuple& tuple, b2World* world);

		static void DeleteBody(Body2DComponent* body, b2World* world);

		/// Joint Factory

		static const bool BindJoint(Body2DComponent* bodyA, Body2DComponent* bodyB, JointType type, JointInfo* info, b2World* world);

		static const bool DeleteJoint(Body2DComponent* body, b2World* word);

		/// Raycating

		static const RayCast2DHitInfo& RayCast(const glm::vec2& startPoisition, const glm::vec2& targerPosition, b2World* world);

		static const std::vector<RayCast2DHitInfo> CircleCast(const glm::vec2& startPoisition, float distance, b2World* world);

		/// Internal

		static b2BodyType FindType(uint16_t type);

	private:

		/// Body Types

		static void CreateStatic(Body2D* bodyDef);

		static void CreateKinematic(Body2D* bodyDef);

		static void CreateDynamic(Body2D* bodyDef);

		/// Joint Types

		static bool CreateDistanceJoint(Body2D* bodyA, Body2D* bodyB, DistanceJointInfo* info, b2World* world);

		static bool CreateRevoluteJoint(Body2D* bodyA, Body2D* bodyB, RevoluteJointInfo* info, b2World* world);

		static bool CreatePrismaticJoint(Body2D* bodyA, Body2D* bodyB, PrismaticJointInfo* info, b2World* world);

		static bool CreateRopeJoint(Body2D* bodyA, Body2D* bodyB, RopeJointInfo* info, b2World* world);

		/// 

		static void SetTransfrom(PhysicsBaseTuple& tuple);

	private:

		friend class EditorLayer;

		friend class Scene;
	};
}