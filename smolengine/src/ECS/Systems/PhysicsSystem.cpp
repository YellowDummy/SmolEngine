#include "stdafx.h"
#include "ECS/Systems/PhysicsSystem.h"
#include "ECS/WorldAdmin.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/RigidbodyComponent.h"

#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"
#include "ECS/Components/Singletons/Bullet3WorldSComponent.h"
#include "ECS/Systems/JobsSystem.h"

#include <Frostium3D/DebugRenderer.h>
#include <btBulletDynamicsCommon.h>


namespace SmolEngine
{
	void PhysicsSystem::SetLinearVelocity(RigidbodyComponent* component, const glm::vec3& dir)
	{
		btRigidBody* rb = component->m_Body;
		rb->activate(true);
		rb->setLinearVelocity(btVector3(dir.x, dir.y, dir.z));
	}

	void PhysicsSystem::SetAngularFactor(RigidbodyComponent* component, const glm::vec3& axis)
	{
		btRigidBody* rb = component->m_Body;
		rb->activate(true);
		rb->setAngularFactor(btVector3(axis.x, axis.y, axis.z));
	}

	void PhysicsSystem::AddForce(RigidbodyComponent* component, const glm::vec3& dir)
	{
		btRigidBody* rb = component->m_Body;
		rb->activate(true);
		rb->applyCentralForce(btVector3(dir.x, dir.y, dir.z));
	}

	void PhysicsSystem::AddImpulse(RigidbodyComponent* component, const glm::vec3& dir)
	{
		btRigidBody* rb = component->m_Body;
		rb->activate(true);
		rb->applyCentralImpulse(btVector3(dir.x, dir.y, dir.z));
	}

	void PhysicsSystem::AddTorque(RigidbodyComponent* component, const glm::vec3& torque)
	{
		btRigidBody* rb = component->m_Body;
		rb->activate(true);
		rb->applyTorque(btVector3(torque.x, torque.y, torque.z));
	}

	void PhysicsSystem::OnBeginWorld()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;

		const auto& dynamic_group = m_World->m_CurrentRegistry->view<TransformComponent, RigidbodyComponent>();
		for (const auto& entity : dynamic_group)
		{
			const auto& [transform, rigidbodyComponent] = dynamic_group.get<TransformComponent, RigidbodyComponent>(entity);
			rigidbodyComponent.Create(&rigidbodyComponent.CreateInfo, transform.WorldPos, transform.Rotation);

			AttachBodyToActiveScene(dynamic_cast<RigidActor*>(&rigidbodyComponent));
		}
	}

	void PhysicsSystem::OnEndWorld()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;

		{
			btDiscreteDynamicsWorld* world = m_State->World;

			// remove the rigidbodies from the dynamics world and delete them
			for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
			{
				btCollisionObject* obj = world->getCollisionObjectArray()[i];
				btRigidBody* body = btRigidBody::upcast(obj);
				if (body && body->getMotionState())
				{
					delete body->getMotionState();
				}

				world->removeCollisionObject(obj);
				delete obj;
			}
		}

		// delete collision shapes and rigidbodies
		const auto& dynamic_group = reg->view<RigidbodyComponent>();
		for (const auto& entity : dynamic_group)
		{
			auto& body = dynamic_group.get<RigidbodyComponent>(entity);

			btCollisionShape* shape = body.m_Shape;
			delete shape;
			body.SetActive(false);
		}
	}

	void PhysicsSystem::OnUpdate(float delta)
	{
		m_State->World->stepSimulation(1.f / 60.0f, 2, delta);
	}

	void PhysicsSystem::UpdateTransforms()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		const auto& dynamic_group = m_World->m_CurrentRegistry->view<TransformComponent, RigidbodyComponent>();

		JobsSystem::BeginSubmition();
		for (const auto& entity : dynamic_group)
		{
			auto& transform = dynamic_group.get<TransformComponent>(entity);
			const auto& rigidbodyComponent = dynamic_group.get<RigidbodyComponent>(entity);

			JobsSystem::Schedule([&transform, &rigidbodyComponent]()
			{
				btRigidBody* body = rigidbodyComponent.m_Body;
				btTransform btTransf;
				body->getMotionState()->getWorldTransform(btTransf);

				RigidActor::BulletToGLMTransform(&btTransf, transform.WorldPos, transform.Rotation);
			});
		}
		JobsSystem::EndSubmition();
	}

	void PhysicsSystem::AttachBodyToActiveScene(RigidActor* body)
	{
		btRigidBody* bd = body->m_Body;
		m_State->World->addRigidBody(bd);
	}
}
