#include "stdafx.h"
#include "Physics/Bullet3/RigidActor.h"
#include "ECS/Components/Singletons/Bullet3WorldSComponent.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace SmolEngine
{
	void RigidActor::GLMToBulletTransform(const glm::vec3& pos, const glm::vec3& rot, btTransform* transform)
	{
		glm::quat q = glm::quat(rot);


		transform->setIdentity();
		transform->setOrigin({ pos.x, pos.y, pos.z });
		transform->setRotation({ q.x, q.y, q.z, q.w });
	}

	void RigidActor::BulletToGLMTransform(const btTransform* transform, glm::vec3& pos, glm::vec3& rot)
	{
		const auto& origin = transform->getOrigin();
		const auto& rotation = transform->getRotation();

		pos.x = origin.x();
		pos.y = origin.y();
		pos.z = origin.z();

		glm::quat q = glm::quat(cos(rotation.getAngle() / 2),
			rotation.getAxis().getX() * sin(rotation.getAngle() / 2),
			rotation.getAxis().getY() * sin(rotation.getAngle() / 2),
			rotation.getAxis().getZ() * sin(rotation.getAngle() / 2));

		rot = glm::eulerAngles(q);
	}

	bool RigidActor::IsActive() const
	{
		return m_Active;
	}

	void RigidActor::InitBase(BodyCreateInfo* info)
	{
		switch (info->eShape)
		{
		case RigidBodyShape::Box:     CreateBox(info); break;
		case RigidBodyShape::Sphere:  CreateSphere(info); break;
		case RigidBodyShape::Capsule: CreateCapsule(info); break;
		}

		if (info->StateIndex == 1) // static
			info->Mass = 0.0f;

		bool isDynamic = info->Mass != 0.0f;
		if(isDynamic)
			m_Shape->calculateLocalInertia(info->Mass, btVector3(info->LocalInertia.x, info->LocalInertia.y, info->LocalInertia.z));
	}

	void RigidActor::CreateCapsule(BodyCreateInfo* info)
	{
	}

	void RigidActor::CreateSphere(BodyCreateInfo* info)
	{

	}

	void RigidActor::CreateBox(BodyCreateInfo* info)
	{
		m_Shape = new btBoxShape(btVector3(info->BoxShapeInfo.X, info->BoxShapeInfo.Y, info->BoxShapeInfo.Z));
	}

	void RigidActor::SetActive(bool value)
	{
		m_Active = value;
	}
}