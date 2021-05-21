#include "stdafx.h"
#include "ECS/Components/Singletons/Bullet3WorldSComponent.h"

#include <btBulletDynamicsCommon.h>

namespace SmolEngine
{
	Bullet3WorldSComponent* Bullet3WorldSComponent::Instance = nullptr;

	Bullet3WorldSComponent::Bullet3WorldSComponent()
	{
		PhysicsContextCreateInfo info{};
		Init(&info);
	}

	Bullet3WorldSComponent::Bullet3WorldSComponent(PhysicsContextCreateInfo* info)
	{
		Init(info);
	}

	Bullet3WorldSComponent::~Bullet3WorldSComponent()
	{
	}

	void Bullet3WorldSComponent::Init(PhysicsContextCreateInfo* info)
	{
		Config = new btDefaultCollisionConfiguration();
		Dispatcher = new btCollisionDispatcher(Config);
		Broadphase = new btDbvtBroadphase();
		Solver = new btSequentialImpulseConstraintSolver;

		World = new btDiscreteDynamicsWorld(Dispatcher, Broadphase, Solver, Config);
		World->setGravity(btVector3(info->Gravity.x, info->Gravity.y, info->Gravity.z));

		Instance = this;
	}
}