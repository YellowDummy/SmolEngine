#include "stdafx.h"
#include "ECS/Systems/PhysicsSystem.h"
#include "ECS/WorldAdmin.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/RigidbodyComponent.h"
#include "ECS/Components/StaticBodyComponent.h"
#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"
#include "ECS/Components/Singletons/PhysXWorldSComponent.h"

#include <PxScene.h>
#include <PxPhysics.h>
#include <PxSceneDesc.h>
#include <PxRigidBody.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultSimulationFilterShader.h>

namespace SmolEngine
{
	void PhysicsSystem::SetLinearDamping(RigidbodyComponent* component, float value)
	{
		physx::PxRigidBody* body = component->DynamicBody.m_DynamicBody;
		body->setLinearDamping(value);
	}

	void PhysicsSystem::SetAngularDamping(RigidbodyComponent* component, float value)
	{
		physx::PxRigidBody* body = component->DynamicBody.m_DynamicBody;
		body->setAngularDamping(value);
	}

	void PhysicsSystem::SetLinearVelocity(RigidbodyComponent* component, const glm::vec3& dir)
	{
		physx::PxRigidBody* body = component->DynamicBody.m_DynamicBody;
		body->setLinearVelocity({ dir.x, dir.y, dir.z });
	}

	void PhysicsSystem::AddForce(RigidbodyComponent* component, const glm::vec3& dir)
	{
		physx::PxRigidBody* body = component->DynamicBody.m_DynamicBody;
		body->addForce({ dir.x, dir.y, dir.z });
	}

	void PhysicsSystem::AddImpulse(RigidbodyComponent* component, const glm::vec3& dir)
	{
		physx::PxRigidBody* body = component->DynamicBody.m_DynamicBody;
		body->addForce({ dir.x, dir.y, dir.z }, physx::PxForceMode::eIMPULSE);
	}

	void PhysicsSystem::OnBeginWorld()
	{
		CreateScene();
		entt::registry* reg = m_World->m_CurrentRegistry;

		const auto& dynamic_group = m_World->m_CurrentRegistry->view<TransformComponent, RigidbodyComponent>();
		for (const auto& entity : dynamic_group)
		{
			const auto& [transform, rigidbodyComponent] = dynamic_group.get<TransformComponent, RigidbodyComponent>(entity);
			RigidActor* body = &rigidbodyComponent.DynamicBody;
			BodyCreateInfo* info = &rigidbodyComponent.CreateInfo;

			body->Create(info, transform.WorldPos, transform.Rotation);
			AttachBodyToActiveScene(body);
		}

		const auto& static_group = m_World->m_CurrentRegistry->view<TransformComponent, StaticBodyComponent>();
		for (const auto& entity : static_group)
		{
			const auto& [transform, rigidbodyComponent] = static_group.get<TransformComponent, StaticBodyComponent>(entity);
			RigidActor* body = &rigidbodyComponent.StaticBody;
			BodyCreateInfo* info = &rigidbodyComponent.CreateInfo;

			body->Create(info, transform.WorldPos, transform.Rotation);
			AttachBodyToActiveScene(body);
		}
	}

	void PhysicsSystem::OnEndWorld()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;

		const auto& dynamic_group = reg->view<RigidbodyComponent>();
		for (const auto& entity : dynamic_group)
		{
			auto& body = dynamic_group.get<RigidbodyComponent>(entity).DynamicBody;
			body.Destroy();
		}

		const auto& static_group = m_World->m_CurrentRegistry->view<StaticBodyComponent>();
		for (const auto& entity : static_group)
		{
			auto& body = static_group.get<StaticBodyComponent>(entity).StaticBody;
			body.Destroy();
		}

		DeleteScene();
	}

	void PhysicsSystem::CreateScene()
	{
		auto& scale = m_State->mPhysics->getTolerancesScale();
		physx::PxSceneDesc sceneDesc(scale);
		sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(m_State->CreateInfo.NumWorkThreads);
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		m_State->mScene = m_State->mPhysics->createScene(sceneDesc);
	}

	void PhysicsSystem::DeleteScene()
	{
		m_State->mScene->release();
		m_State->mScene = nullptr;
	}

	void PhysicsSystem::AttachBodyToActiveScene(RigidActor* body)
	{
		m_State->mScene->addActor(*body->m_BaseBody);
	}
}
