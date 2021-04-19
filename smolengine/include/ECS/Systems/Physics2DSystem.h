#pragma once
#include "Core/Core.h"
#include "ECS/Components/Singletons/Box2DWorldSComponent.h"

#include <Frostium3D/Common/Time.h>
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <vector>

namespace SmolEngine
{
	struct Box2DWorldSComponent;
	struct TransformComponent;
	struct PhysicsBaseTuple;
	struct Body2DComponent;
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

	private:

		static void OnBegin(Box2DWorldSComponent* data);
		static void OnUpdate(Frostium::DeltaTime delta, uint32_t velocityIterations, uint32_t positionIterations, Box2DWorldSComponent* data);

		// Body Factory
		static void CreateBody(Body2DComponent* body, TransformComponent* tranform, b2World* world, Ref<Actor> actor);
		static void DeleteBodies(entt::registry& registry, b2World* world);

		// Joint Factory
		static const bool BindJoint(Body2DComponent* bodyA, Body2DComponent* bodyB, JointType type, JointInfo* info, b2World* world);
		static const bool DeleteJoint(Body2DComponent* body, b2World* word);

		// Internal
		static b2BodyType FindType(uint16_t type);

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
		static void UpdateTransforms(entt::registry& registry);

	public:

		// Forces
		static void AddForce(Body2DComponent* body, const glm::vec2& force, bool wakeBody = true);
		static void AddForce(Body2DComponent* body, const glm::vec2& force, const glm::vec2& point, bool wakeBody = true);

		// RayCasting
		static const RayCast2DHitInfo RayCast(const glm::vec2& startPoisition, const glm::vec2& targerPosition);
		static void CircleCast(const glm::vec2& startPoisition, const float distance, std::vector<RayCast2DHitInfo>& outHits);

	private:

		friend class EditorLayer;
		friend class WorldAdmin;
	};
}