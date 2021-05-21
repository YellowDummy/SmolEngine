#include "Scripts/BasePlayerScript.h"

#include <Frostium3D/Common/SLog.h>
#include <Frostium3D/Common/Input.h>

#include "ECS/ComponentsCore.h"
#include "ECS/Systems/PhysicsSystem.h"

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
			if (Frostium::Input::IsKeyPressed(Frostium::KeyCode::G))
			{
				PhysicsSystem::AddForce(rigid, { 0, -(*m_Speed * 10), 0 });
			}
			if (Frostium::Input::IsKeyPressed(Frostium::KeyCode::T))
			{
				PhysicsSystem::AddForce(rigid, { 0, *m_Speed * 10, 0 });
			}
			if (Frostium::Input::IsKeyPressed(Frostium::KeyCode::F))
			{
				PhysicsSystem::AddForce(rigid, { 0, 0, (*m_Speed * 10) / 2.0f,});
			}
			if (Frostium::Input::IsKeyPressed(Frostium::KeyCode::H))
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