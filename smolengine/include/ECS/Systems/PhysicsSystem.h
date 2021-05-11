#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	struct PhysXWorldSComponent;
	struct WorldAdminStateSComponent;
	struct RigidbodyComponent;
	class RigidActor;

	class PhysicsSystem
	{
	public:

		static void SetLinearDamping(RigidbodyComponent* component, float value);
		static void SetAngularDamping(RigidbodyComponent* component, float value);
		static void SetLinearVelocity(RigidbodyComponent* component, const glm::vec3& dir);

		static void AddForce(RigidbodyComponent* component, const glm::vec3& dir);
		static void AddImpulse(RigidbodyComponent* component, const glm::vec3& dir);

	private:

		static void OnBeginWorld();
		static void OnEndWorld();

		static void CreateScene();
		static void DeleteScene();
		static void AttachBodyToActiveScene(RigidActor* body);

	private:

		inline static PhysXWorldSComponent*       m_State = nullptr;
		inline static WorldAdminStateSComponent*  m_World = nullptr;

		friend class WorldAdmin;
	};
}