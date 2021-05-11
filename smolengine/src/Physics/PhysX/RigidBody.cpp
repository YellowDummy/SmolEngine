#include "stdafx.h"
#include "Physics/PhysX/RigidBody.h"
#include "ECS/Components/Singletons/PhysXWorldSComponent.h"

#include <PxPhysics.h>
#include <PxRigidDynamic.h>
#include <PxMaterial.h>

namespace SmolEngine
{
	void RigidBody::Create(BodyCreateInfo* info, const glm::vec3& pos, const glm::vec3& rot)
	{
		if (info->pActor && m_BaseBody == nullptr)
		{
			physx::PxTransform transform{};
			GlmToPxTransform(pos, rot, &transform);

			m_DynamicBody = m_Engine->mPhysics->createRigidDynamic(transform);
			m_BaseBody = m_DynamicBody;
			m_BaseBody->userData = info->pActor;

			InitRigidActor(info, m_BaseBody);
		}
	}

	void RigidBody::Destroy()
	{
		if (m_BaseBody->isReleasable())
		{
			m_BaseBody->release();

			m_BaseBody = nullptr;
			m_DynamicBody = nullptr;
		}
	}
}