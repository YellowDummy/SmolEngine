#pragma once

#include "Core/Core.h"
#include "Physics/Bullet3/RigidActor.h"

namespace SmolEngine
{
	class Actor;

	class RigidBody: public RigidActor
	{
		void Create(BodyCreateInfo* info, const glm::vec3& pos, const glm::vec3& rot) override;

	private:

		friend class PhysicsSystem;
		friend class ComponentHandler;
	};
}