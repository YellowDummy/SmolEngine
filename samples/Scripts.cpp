#include "Scripts.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

RigidbodyComponent* rigid = nullptr;
CameraComponent* camera = nullptr;
TransformComponent* transform = nullptr;
TransformComponent* cameraTransform = nullptr;

BasePlayerScript::BasePlayerScript()
{
	CreateValue<float>(22, "Speed");
	CreateValue<float>(5, "Camera Speed");
}

void BasePlayerScript::OnProcess(float deltaTime)
{
	glm::quat q = glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	glm::vec3 upDir = glm::rotate(q, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 forwardDir = glm::rotate(q, glm::vec3(0.0f, 0.0f, -1.0f));
	glm::vec3 rightDir = glm::rotate(q, glm::vec3(1.0f, 0.0f, 0.0f));

	// Updates Pos
	{
		if (rigid)
		{
			if (Input::IsKeyPressed(KeyCode::W))
			{
				PhysicsSystem::SetLinearVelocity(rigid, (forwardDir * deltaTime) * (*m_Speed * 2));
			}

			if (Input::IsKeyPressed(KeyCode::S))
			{
				PhysicsSystem::SetLinearVelocity(rigid, (-forwardDir * deltaTime) * (*m_Speed * 2));
			}

			if (Input::IsKeyPressed(KeyCode::D))
			{
				PhysicsSystem::SetLinearVelocity(rigid, (rightDir * deltaTime) * (*m_Speed * 2));
			}

			if (Input::IsKeyPressed(KeyCode::A))
			{
				PhysicsSystem::SetLinearVelocity(rigid, (-rightDir * deltaTime) * (*m_Speed * 2));
			}

			if (Input::IsKeyPressed(KeyCode::Space))
			{
				PhysicsSystem::SetLinearVelocity(rigid, (glm::vec3(0, 1, 0) * deltaTime) * (*m_Speed * 10));
			}
		}
	}

	// Updates Camera
	{
		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		glm::vec2 deltaT = (mouse - m_InitialMousePosition) * 0.003f;
		m_InitialMousePosition = mouse;

		float yawSign = upDir.y;
		m_Yaw += yawSign * deltaT.x * 2;
		m_Pitch += deltaT.y * 2;

		cameraTransform->Rotation = { -m_Pitch, -m_Yaw, 0.0f };
	}
}

void BasePlayerScript::OnBegin()
{
	m_Speed = GetValue<float>("Speed");
	m_CameraSpeed = GetValue<float>("Camera Speed");

	transform = GetComponent<TransformComponent>();
	rigid = GetComponent<RigidbodyComponent>();

	cameraTransform = GetChildByName("Camera")->GetComponent<TransformComponent>();
	camera = GetChildByName("Camera")->GetComponent<CameraComponent>();

	PhysicsSystem::SetAngularFactor(rigid, { 0, 1, 0 });
}

void BasePlayerScript::OnDestroy()
{

}

void BasePlayerScript::OnCollisionContact()
{

}

void BasePlayerScript::OnCollisionExit()
{

}