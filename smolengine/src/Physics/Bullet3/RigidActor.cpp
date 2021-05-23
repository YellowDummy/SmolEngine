#include "stdafx.h"
#include "Physics/Bullet3/RigidActor.h"
#include "ECS/Components/Singletons/Bullet3WorldSComponent.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace SmolEngine
{
	void RigidActor::GLMToBulletTransform(const glm::vec3& pos, const glm::vec3& rot, btTransform* transform)
	{
		glm::mat4 model;
		{
			const glm::mat4 rotation = glm::orientate4(rot);
			model = glm::translate(glm::mat4(1.0f), pos) * rotation * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
		}

		transform->setIdentity();
		transform->setFromOpenGLMatrix(glm::value_ptr(model));

	}

	void RigidActor::BulletToGLMTransform(const btTransform* transform, glm::vec3& pos, glm::vec3& rot)
	{
		const auto& origin = transform->getOrigin();
		float x, y, z;
		{
			const auto& rotation = transform->getRotation();
			const glm::mat4 m = glm::toMat4(glm::quat(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ()));
			glm::extractEulerAngleXYZ(m, x, y, z);
		}

		pos.x = origin.x();
		pos.y = origin.y();
		pos.z = origin.z();
		rot = glm::vec3(x, y, z);
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
		btVector3 inertia{};
		if(isDynamic)
			m_Shape->calculateLocalInertia(info->Mass, inertia);

		info->LocalInertia.x = inertia.x();
		info->LocalInertia.y = inertia.y();
		info->LocalInertia.z = inertia.z();
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