#pragma once

// 16.09.2020
//
// Scripting using C++

#include <SmolEngineCore.h>

using namespace SmolEngine;

class CharMovementScript : public ScriptableObject
{
public:

	//Must be implemented by the user in order to register an external script in the engine
	std::shared_ptr<ScriptableObject> Instantiate() override
	{
		return std::make_shared<CharMovementScript>();
	}

	//Default constructor must be implemented!
	CharMovementScript()
	{
		//Exposes a property to the editor
		OUT_FLOAT("Speed", &speed);
		OUT_INT("SpeedMod", &speedMod);
		OUT_STRING("Name", &name);
	}

	void Start() override
	{
		CONSOLE_WARN("Player name is: " + name);

		rb = &GetComponent<Rigidbody2DComponent>();

		for (auto obj : GetActorList())
		{
			CONSOLE_INFO(std::string("Actor found : ") + obj->GetName());
		}

		for (auto obj : GetActorListByTag("Default"))
		{
			CONSOLE_INFO(std::string("Actor ID find by tag: ") + std::to_string(obj->GetID()));
		}

		auto ground = FindChildByName("Ground");
		if (ground)
		{
			CONSOLE_INFO(std::string("Child found : ") + ground->GetName());
		}

	}

	void OnUpdate(DeltaTime deltaTime) override
	{
		if (Input::IsKeyPressed(KeyCode::E))
		{
			rb->AddForce({ 20.0f * (speed * speedMod), 0.0f });
		}

		if (Input::IsKeyPressed(KeyCode::Q))
		{
			rb->AddForce({ -20.0f * (speed * speedMod), 0.0f });
		}

		if (Input::IsKeyPressed(KeyCode::Space))
		{
			rb->AddForce({ 0.0f, 50.0f * (speed * speedMod) });
		}
	}

	void OnDestroy() override {}

private:
	Rigidbody2DComponent* rb = nullptr;

	float speed = 1.0f;
	int speedMod = 1;
	std::string name = "Noob99";
};

class CameraMovementScript : public ScriptableObject
{
public:

	//Must be implemented by the user in order to register an external script in the engine
	std::shared_ptr<ScriptableObject> Instantiate() override
	{
		return std::make_shared<CameraMovementScript>();
	}

	//Default constructor must be implemented
	CameraMovementScript() { OUT_FLOAT("CameraSpeed", &m_CameraSpeed); }

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

private:
	float m_CameraSpeed = 0.5f;
	Ref<Actor> m_Player;
};