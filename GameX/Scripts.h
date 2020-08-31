#pragma once

#include "SmolEngineCore.h"
#include "Core/ECS/ScriptableObject.h"
#include <Core/ECS/Components.h>

#include "../SmolEngine-Editor/Source/EditorLayer.h"

using namespace SmolEngine;

//Examples Of Scripting In C++

class CharMovementScript : public ScriptableObject
{
public:

	CharMovementScript(Ref<Actor> actor)
		:ScriptableObject(actor) {}

	void Start() override
	{
		rb = &GetComponent<Rigidbody2DComponent>();

		for (auto obj : GetActorList())
		{
			CONSOLE_INFO(std::string("Actor found : ") + obj->GetName());
		}

		for (auto obj : GetActorListByTag("Default"))
		{
			NATIVE_INFO("Actor ID find by tag: {}", obj->GetID());
		}

		auto ground = FindChildByName("Ground");
		if (ground)
		{
			CONSOLE_INFO(std::string("Child found : ") + ground->GetName());
		}

	}

	void OnUpdate(DeltaTime deltaTime) override
	{
		if (Input::IsKeyPressed(KeyCode::Q))
		{
			rb->AddForce({ 0.0f, 50.0f });
		}
	}

	void OnDestroy() override {}

private:
	Rigidbody2DComponent* rb = nullptr;
};

class CameraMovementScript : public ScriptableObject
{
public:

	CameraMovementScript(Ref<Actor> actor)
		:ScriptableObject(actor) {}


	void Start() override
	{
		m_Player = FindActorByTag("Player");
		if (m_Player == nullptr) { CONSOLE_ERROR("Player not found!"); }
	}


	void OnUpdate(DeltaTime deltaTime) override
	{
		auto& playerPos = m_Player->GetComponent<TransfromComponent>().WorldPos;
		auto& cameraPos = GetComponent<TransfromComponent>().WorldPos;

		int distanceY = playerPos.y - cameraPos.y;
		int distanceX = playerPos.x - cameraPos.x;

		if (distanceY > 0.3 || distanceX > 0.3 || distanceY < -0.3f || distanceX < -0.3f)
		{
			m_CameraSpeed += 0.05f;
		}
		else
		{
			m_CameraSpeed = 0.05f;
		}

		if (playerPos.x > cameraPos.x)
		{
			cameraPos.x += m_CameraSpeed * deltaTime;
		}

		if (playerPos.x > cameraPos.x)
		{
			cameraPos.x -= m_CameraSpeed * deltaTime;
		}

		if (playerPos.y > cameraPos.y)
		{
			cameraPos.y += m_CameraSpeed * deltaTime;
		}

		if (playerPos.y < cameraPos.y)
		{
			cameraPos.y -= m_CameraSpeed * deltaTime;
		}
	}


	void OnDestroy() override  {}

private:
	float m_CameraSpeed = 0.5f;
	Ref<Actor> m_Player;
};