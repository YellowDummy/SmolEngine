#pragma once

// 04.10.2020
//
// Scripting using C++

#include <SmolEngineCore.h>
#include <functional>

using namespace SmolEngine;

class CharMovementScript : public ScriptableObject
{
public:

	//Must be implemented by the user in order to register an external script in the engine (!!)
	std::shared_ptr<ScriptableObject> Instantiate() override
	{
		return std::make_shared<CharMovementScript>();
	}

	//Default constructor must be implemented
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

		if (HasComponent<Rigidbody2DComponent>())
		{
			rb = &GetComponent<Rigidbody2DComponent>();
		}

		if (HasComponent<CanvasComponent>())
		{
			auto& canvas = GetComponent<CanvasComponent>();

			if (canvas.IsValid())
			{
				const auto button = canvas.GetButton(0); // 0 is index inside canvas component
				if (button)
				{
					button->SetOnClickCallback(std::bind(&CharMovementScript::ButtonClickCallback, this));
				}

				const auto text = canvas.GetTextLabel(1);
				if (text)
				{
					text->SetText("GAME STARTED");
					text->SetColor({ 0.4f, 0.3f, 0.6f, 1.0f });
				}
			}
		}

		for (const auto obj : GetActorList())
		{
			CONSOLE_INFO(std::string("Actor found : ") + obj->GetName());
		}

		for (const auto obj : GetActorListByTag("Default"))
		{
			CONSOLE_INFO(std::string("IDs by tag <Default>: ") + std::to_string(obj->GetID()));
		}

		const auto ground = FindChildByName("Ground");
		if (ground)
		{
			CONSOLE_INFO(std::string("Child found : ") + ground->GetName());
		}

	}

	void ButtonClickCallback()
	{
		CONSOLE_INFO(std::string("Button Is Pressed!"));
	}

	void OnUpdate(DeltaTime deltaTime) override
	{
		if (rb == nullptr) { return; }

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

	//Must be implemented by the user in order to register an external script in the engine!
	std::shared_ptr<ScriptableObject> Instantiate() override
	{
		return std::make_shared<CameraMovementScript>();
	}

	//Default constructor must be implemented
	CameraMovementScript() 
	{ 
		OUT_FLOAT("CameraSpeed", &m_CameraSpeed); 
	}

	void Start() override
	{
		m_Player = FindActorByTag("Player");

		if (!m_Player) { CONSOLE_ERROR("Player not found!"); }
	}

	void OnUpdate(DeltaTime deltaTime) override
	{
		if (!m_Player) { return; }

		auto& playerPos = m_Player->GetComponent<TransformComponent>().WorldPos;
		auto& cameraPos = GetComponent<TransformComponent>().WorldPos;

		float distanceY = playerPos.y - cameraPos.y;
		float distanceX = playerPos.x - cameraPos.x;

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