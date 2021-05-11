#include "stdafx.h"
#include "Physics/PhysX/RigidActor.h"
#include "ECS/Components/Singletons/PhysXWorldSComponent.h"

#include <PxShape.h>
#include <PxMaterial.h>
#include <extensions/PxRigidActorExt.h>
#include <extensions/PxRigidBodyExt.h>

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
		m_Engine = PhysXWorldSComponent::Get();

		physx::PxShape* shape = nullptr;
		physx::PxMaterial* material = m_Engine->mPhysics->createMaterial(info->Material.StaticFriction,
			info->Material.DynamicFriction, info->Material.Restitution);

		ShapeCreateInfo shapeCI;
		shapeCI.Body = physicsActor;
		shapeCI.BodyInfo = info;
		shapeCI.Material = material;
		shapeCI.Shape = shape;

		switch (info->eShape)
		{
		case RigidBodyShape::Capsule: CreateCapsule(&shapeCI); break;
		case RigidBodyShape::Sphere: CreateSphere(&shapeCI); break;
		case RigidBodyShape::Box: CreateBox(&shapeCI); break;
		}

		physx::PxRigidBody* body = dynamic_cast<physx::PxRigidBody*>(physicsActor);
		physx::PxRigidBodyExt::updateMassAndInertia(*body, info->Material.Density);

		shape->release();
		material->release();
	}

	void RigidActor::CreateCapsule(ShapeCreateInfo* info)
	{

	}

	void RigidActor::CreateSphere(ShapeCreateInfo* info)
	{

	}

	void RigidActor::CreateBox(ShapeCreateInfo* info)
	{
		info->Shape = physx::PxRigidActorExt::createExclusiveShape(*info->Body, physx::PxBoxGeometry(info->BodyInfo->BoxShapeInfo.X / 2, 
			info->BodyInfo->BoxShapeInfo.Y / 2, info->BodyInfo->BoxShapeInfo.Z / 2), *info->Material);

		info->Body->attachShape(*info->Shape);
	}

}