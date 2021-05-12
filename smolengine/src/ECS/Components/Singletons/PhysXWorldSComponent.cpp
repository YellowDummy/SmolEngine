#include "stdafx.h"
#include "ECS/Components/Singletons/PhysXWorldSComponent.h"
#include "Physics/PhysX/PhysXBase.h"

#include <PxFoundation.h>
#include <PxPhysics.h>
#include <PxPhysicsVersion.h>
#include <PxMaterial.h>
#include <common/PxTolerancesScale.h>

namespace SmolEngine
{
	PhysXWorldSComponent* PhysXWorldSComponent::Instance = nullptr;

	PhysXWorldSComponent::PhysXWorldSComponent()
	{
		PhysicsContextCreateInfo info{};
		Init(&info);
	}

	PhysXWorldSComponent::PhysXWorldSComponent(PhysicsContextCreateInfo* info)
	{
		Init(info);
	}

	PhysXWorldSComponent::~PhysXWorldSComponent()
	{
		mFoundation->release();
		mPhysics->release();
	}

	void PhysXWorldSComponent::Init(PhysicsContextCreateInfo* info)
	{
		mAllocator = std::make_shared<PhysXAllocator>();
		mErrorCallback = std::make_shared<PhysXErrorCallback>();

		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *mAllocator.get(), *mErrorCallback.get());
		physx::PxTolerancesScale scale{};
		scale.length = 100;
		scale.speed = info->Speed;

		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, scale);
		mDefaultMaterial = mPhysics->createMaterial(0.5f,
			0.5f, 0.6f);

		mDefaultMaterial->setFrictionCombineMode(physx::PxCombineMode::eMULTIPLY);
		Instance = this;
	}
}