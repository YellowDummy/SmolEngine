#include "stdafx.h"
#include "ECS/Systems/PhysicsSystem.h"
#include "ECS/WorldAdmin.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/RigidbodyComponent.h"

#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"
#include "ECS/Components/Singletons/Bullet3WorldSComponent.h"

#include <Frostium3D/DebugRenderer.h>
#include <btBulletDynamicsCommon.h>


namespace SmolEngine
{
	void PhysicsSystem::SetLinearVelocity(RigidbodyComponent* component, const glm::vec3& dir)
	{
		btRigidBody* rb = component->Body.m_Body;
		rb->activate(true);
		rb->setLinearVelocity(btVector3(dir.x, dir.y, dir.z));
	}

	void PhysicsSystem::AddForce(RigidbodyComponent* component, const glm::vec3& dir)
	{
		btRigidBody* rb = component->Body.m_Body;
		rb->activate(true);
		rb->applyCentralForce(btVector3(dir.x, dir.y, dir.z));
	}

	void PhysicsSystem::AddImpulse(RigidbodyComponent* component, const glm::vec3& dir)
	{
		btRigidBody* rb = component->Body.m_Body;
		rb->activate(true);
		rb->applyCentralImpulse(btVector3(dir.x, dir.y, dir.z));
	}

	void PhysicsSystem::AddTorque(RigidbodyComponent* component, const glm::vec3& torque)
	{
		btRigidBody* rb = component->Body.m_Body;
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
			auto body = &rigidbodyComponent.Body;
			body->Create(&rigidbodyComponent.CreateInfo, transform.WorldPos, transform.Rotation);
			AttachBodyToActiveScene(body);
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
			auto& body = dynamic_group.get<RigidbodyComponent>(entity).Body;

			btCollisionShape* shape = body.m_Shape;
			delete shape;
			body.SetActive(false);
		}
	}

	void PhysicsSystem::OnUpdate(float delta)
	{
		constexpr float simTime = 1.0f / 60.0f;
		m_State->World->stepSimulation(simTime, 10);
	}

	void PhysicsSystem::UpdateTransforms()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;

		const auto& dynamic_group = m_World->m_CurrentRegistry->view<TransformComponent, RigidbodyComponent>();
		for (const auto& entity : dynamic_group)
		{
			const auto& [transform, rigidbodyComponent] = dynamic_group.get<TransformComponent, RigidbodyComponent>(entity);
			btRigidBody* body = rigidbodyComponent.Body.m_Body;
			btTransform btTransf;
			body->getMotionState()->getWorldTransform(btTransf);

			RigidActor::BulletToGLMTransform(&btTransf, transform.WorldPos, transform.Rotation);
		}
	}

	void PhysicsSystem::AttachBodyToActiveScene(RigidActor* body)
	{
		btRigidBody* bd = body->m_Body;
		m_State->World->addRigidBody(bd);
	}

	void PhysicsSystem::DebugDraw()
	{
		Frostium::DebugRenderer::BeginDebug();
		{
			m_State->World->debugDrawWorld();
		}
		Frostium::DebugRenderer::EndDebug();
	}
}
