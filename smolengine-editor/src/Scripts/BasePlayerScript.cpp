#include "Scripts/BasePlayerScript.h"
#include "SmolEngineCore.h"

namespace SmolEngine
{
	RigidbodyComponent* rigid = nullptr;

	BasePlayerScript::BasePlayerScript()
	{
		CreateValue<float>("Speed");
		CreateValue<int32_t>("Level");
	}

	void BasePlayerScript::OnProcess(float deltaTime)
	{
		if (rigid)
		{
			if (Input::IsKeyPressed(KeyCode::G))
			{
				PhysicsSystem::AddForce(rigid, { 0, -(*m_Speed * 10), 0 });
			}
			if (Input::IsKeyPressed(KeyCode::T))
			{
				PhysicsSystem::AddForce(rigid, { 0, *m_Speed * 10, 0 });
			}
			if (Input::IsKeyPressed(KeyCode::F))
			{
				PhysicsSystem::AddForce(rigid, { 0, 0, (*m_Speed * 10) / 2.0f,});
			}
			if (Input::IsKeyPressed(KeyCode::H))
			{
				PhysicsSystem::AddForce(rigid, { (*m_Speed * 10) / 2.0f, 0, 0 });
			}
		}
	}

	void BasePlayerScript::OnBegin()
	{
		m_Speed = GetValue<float>("Speed");
		m_Level = GetValue<int32_t>("Level");

		if (HasComponent<RigidbodyComponent>())
		{
			rigid = GetComponent<RigidbodyComponent>();
		}
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
}