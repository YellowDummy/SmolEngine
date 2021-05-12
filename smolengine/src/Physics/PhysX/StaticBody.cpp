#include "stdafx.h"
#include "Physics/PhysX/StaticBody.h"
#include "ECS/Components/Singletons/PhysXWorldSComponent.h"

#include <PxPhysics.h>
#include <PxRigidStatic.h>
#include <PxMaterial.h>

namespace SmolEngine
{
	void StaticBody::Create(BodyCreateInfo* info, const glm::vec3& pos, const glm::vec3& rot)
	{
		if (info->pActor && m_BaseBody == nullptr)
		{
			m_Engine = PhysXWorldSComponent::Get();

			physx::PxTransform transform{};
			GlmToPxTransform(pos, rot, &transform);

			m_StaticBody = m_Engine->mPhysics->createRigidStatic(transform);
			m_BaseBody = m_StaticBody;
			m_BaseBody->userData = info->pActor;

			InitRigidActor(info, m_BaseBody);
		}
	}

	void StaticBody::Destroy()
	{
		if (m_BaseBody->isReleasable())
		{
			m_BaseBody->release();

			m_BaseBody = nullptr;
			m_StaticBody = nullptr;
		}
	}
}