#pragma once

#include "Core/Core.h"
#include "Physics/PhysX/RigidActor.h"

namespace SmolEngine
{
	class Actor;

	class RigidBody: public RigidActor
	{
		void Create(BodyCreateInfo* info, const glm::vec3& pos, const glm::vec3& rot) override;
		void Destroy() override;

	private:

		physx::PxRigidDynamic*  m_DynamicBody = nullptr;
		friend class PhysicsSystem;
	};
}