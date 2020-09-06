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

	//Default constructor must be implemented
	CharMovementScript() {}

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
			rb->AddForce({ 20.0f, 0.0f });
		}

		if (Input::IsKeyPressed(KeyCode::Space))
		{
			rb->AddForce({ 0.0f, 50.0f });
		}
	}

	void OnDestroy() override {}

	//Must be implemented by the user in order to register an external script in the engine
	std::shared_ptr<ScriptableObject> Instantiate() override
	{
		return std::make_shared<CharMovementScript>();
	}

private:
	Rigidbody2DComponent* rb = nullptr;
};

class CameraMovementScript : public ScriptableObject
{
public:

	//Default constructor must be implemented
	CameraMovementScript() {}


	void Start() override
	{
		m_Player = FindActorByTag("Player");
		if (m_Player == nullptr) { CONSOLE_ERROR("Player not found!"); }
	}


	void OnUpdate(DeltaTime deltaTime) override
	{
		if (m_Player == nullptr) { return; }

		auto& playerPos = m_Player->GetComponent<TransformComponent>().WorldPos;
		auto& cameraPos = GetComponent<TransformComponent>().WorldPos;

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

	//Must be implemented by the user in order to register an external script in the engine
	std::shared_ptr<ScriptableObject> Instantiate() override
	{
		return std::make_shared<CameraMovementScript>();
	}

private:
	float m_CameraSpeed = 0.5f;
	Ref<Actor> m_Player;
};