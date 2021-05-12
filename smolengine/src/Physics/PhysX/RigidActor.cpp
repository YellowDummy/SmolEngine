#include "stdafx.h"
#include "Physics/PhysX/RigidActor.h"
#include "ECS/Components/Singletons/PhysXWorldSComponent.h"

#include <PxShape.h>
#include <PxMaterial.h>
#include <extensions/PxRigidActorExt.h>

namespace SmolEngine
{
	bool RigidActor::IsActive() const
	{
		return m_BaseBody != nullptr;
	}

	void RigidActor::GlmToPxTransform(const glm::vec3& pos, const glm::vec3& rot, physx::PxTransform* transform)
	{
		glm::quat q = glm::quat(rot);

		transform->p.x = pos.x;
		transform->p.y = pos.y;
		transform->p.z = pos.z;

		transform->q.x = q.x;
		transform->q.y = q.y;
		transform->q.z = q.z;
		transform->q.w = q.w;
	}

	void RigidActor::InitRigidActor(BodyCreateInfo* info, physx::PxRigidActor* physicsActor)
	{
		physx::PxShape* shape = nullptr;

		ShapeCreateInfo shapeCI;
		shapeCI.Body = physicsActor;
		shapeCI.BodyInfo = info;
		shapeCI.Shape = shape;

		switch (info->eShape)
		{
		case RigidBodyShape::Capsule: CreateCapsule(&shapeCI); break;
		case RigidBodyShape::Sphere: CreateSphere(&shapeCI); break;
		case RigidBodyShape::Box: CreateBox(&shapeCI); break;
		}
	}

	void RigidActor::CreateCapsule(ShapeCreateInfo* info)
	{
		info->Shape = physx::PxRigidActorExt::createExclusiveShape(*info->Body,physx::PxCapsuleGeometry(info->BodyInfo->CapsuleShapeInfo.Radius, info->BodyInfo->CapsuleShapeInfo.Height), *m_Engine->mDefaultMaterial);
	}

	void RigidActor::CreateSphere(ShapeCreateInfo* info)
	{
		info->Shape = physx::PxRigidActorExt::createExclusiveShape(*info->Body, physx::PxSphereGeometry(info->BodyInfo->SphereShape.Radius), *m_Engine->mDefaultMaterial);
	}

	void RigidActor::CreateBox(ShapeCreateInfo* info)
	{
		info->Shape = physx::PxRigidActorExt::createExclusiveShape(*info->Body, physx::PxBoxGeometry(info->BodyInfo->BoxShapeInfo.X,
			info->BodyInfo->BoxShapeInfo.Y, info->BodyInfo->BoxShapeInfo.Z), *m_Engine->mDefaultMaterial);
	}

}