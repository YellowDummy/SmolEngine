#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	struct Bullet3WorldSComponent;
	struct WorldAdminStateSComponent;
	struct RigidbodyComponent;
	class RigidActor;

	class PhysicsSystem
	{
	public:

		static void SetLinearVelocity(RigidbodyComponent* component, const glm::vec3& dir);
		static void SetAngularFactor(RigidbodyComponent* component, const glm::vec3& axis);

		static void AddForce(RigidbodyComponent* component, const glm::vec3& dir);
		static void AddImpulse(RigidbodyComponent* component, const glm::vec3& dir);
		static void AddTorque(RigidbodyComponent* component, const glm::vec3& torque);

	private:

		static void OnBeginWorld();
		static void OnEndWorld();
		static void OnUpdate(float delta);

		static void UpdateTransforms();
		static void AttachBodyToActiveScene(RigidActor* body);

	private:

		inline static Bullet3WorldSComponent*       m_State = nullptr;
		inline static WorldAdminStateSComponent*    m_World = nullptr;

		friend class WorldAdmin;
		friend class ComponentHandler;
	};
}