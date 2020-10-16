#pragma once

// 16.10.2020
//
// Scripting using C++

#include <SmolEngineCore.h>
#include <functional>

using namespace SmolEngine;

class CharMovementScript : public ScriptableObject
{
public:

	std::shared_ptr<ScriptableObject> Instantiate() override 	// registers an external script in the engine
	{
		return std::make_shared<CharMovementScript>();
	}

	CharMovementScript() 	// Default constructor must be implemented
	{
		OUT_FLOAT("Speed", &speed); 		// Exposes a property to the editor
		OUT_INT("SpeedMod", &speedMod);
		OUT_STRING("Name", &name);
	}

	void OnBeginPlay() override
	{
		CONSOLE_INFO("Player name is: " + name);

		if (HasComponent<Rigidbody2DComponent>())
		{
			const TransformComponent& transform = GetComponent<TransformComponent>();

			rb = &GetComponent<Rigidbody2DComponent>();

			// Raycasting
			{
				/// Point RayCast

				const RayCast2DHitInfo hitInfo = rb->RayCast(transform.WorldPos, { 0.0f, -10.0f });

				if (hitInfo.IsBodyHitted)
				{
					CONSOLE_WARN("PointRayCast2D: actor " + hitInfo.Actor->GetName() + " hitted");
				}

				/// Circle RayCast

				const std::vector<RayCast2DHitInfo>& hitInfoList = rb->CircleCast(transform.WorldPos, 20.0f);

				for (const auto& info : hitInfoList)
				{
					CONSOLE_WARN("CirlceCast2D: Acrtor " + info.Actor->GetName() + " hitted!");
				}
			}

			// Joints
			{
				const Ref<Actor> testActor = FindActorByName("Player2");

				if (testActor)
				{
					if (testActor->HasComponent<Rigidbody2DComponent>())
					{
						Rigidbody2DComponent& rigid = testActor->GetComponent<Rigidbody2DComponent>();

						/// Rope Example

						RopeJointInfo ropeDef;
						ropeDef.MaxLength = 3.2f;
						ropeDef.CollideConnected = true;

						rb->BindJoint(rigid, JointType::Rope, &ropeDef);

						/// Revolute Example

						RevoluteJointInfo revoluteDef;
						revoluteDef.ReferenceAngle = 30.0f;
						revoluteDef.MotorSpeed = 1.0f;
						revoluteDef.EnableMotor = true;
						revoluteDef.UpperAngle = 2.5f;
						revoluteDef.LowerAngle = 140.0f;
						revoluteDef.MaxMotorTorque = 10.0f;
						revoluteDef.LocalAnchorA = { 1.5f, 1.5f };

						//rb->BindJoint(rigid, JointType::Revolute, &revoluteDef);

						/// Prismatic Example

						PrismaticJointInfo prismaticDef;
						prismaticDef.ReferenceAngle = 30.0f;
						prismaticDef.MotorSpeed = 1.0f;
						prismaticDef.EnableMotor = true;
						prismaticDef.UpperTranslation = 2.5f;
						prismaticDef.LowerTranslation = -5.0f;
						prismaticDef.MaxMotorForce = 10.0f;

						//rb->BindJoint(rigid, JointType::Prismatic, &prismaticDef);
					}
				}
			}
		}

		for (const Ref<Actor> actor : GetActorListByTag("Default"))
		{
			CONSOLE_INFO("IDs by tag <Default>: " + std::to_string(actor->GetID()));
		}

		const Ref<Actor> ground = FindChildByName("Ground");
		if (ground)
		{
			CONSOLE_INFO("Child found : " + ground->GetName());
		}
	}

	void OnUpdate(DeltaTime deltaTime) override
	{
		if (!rb) { return; }

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

	void OnCollisionContact(Actor* actor) override
	{
		CONSOLE_INFO(GetName() + " collided with " + actor->GetName());
	}

	void OnCollisionExit(Actor* actor) override
	{
		if (actor->GetTag() == "Default")
		{
			// Do something
		}
	}

	void OnTriggerContact(Actor* actor) override
	{
		CONSOLE_INFO(GetName() + " entered in trigger " + actor->GetName());
	}

	void OnTriggerExit(Actor* actor) override
	{

	}

	void OnDestroy() override
	{
		
	}

private:

	std::string name = "Noob99";
	float speed = 1.0f;
	int speedMod = 1;

	Rigidbody2DComponent* rb = nullptr;
};

class CameraMovementScript : public ScriptableObject
{
public:

	std::shared_ptr<ScriptableObject> Instantiate() override 	//Must be implemented by the user in order to register an external script in the engine
	{
		return std::make_shared<CameraMovementScript>();
	}

	CameraMovementScript() 	//Default constructor must be implemented
	{ 
		OUT_FLOAT("CameraSpeed", &m_DefaultCameraSpeed);
	}

	void OnBeginPlay() override
	{
		m_Player = FindActorByTag("Player");

		if (m_Player)
		{
			m_PlayerTranform = &m_Player->GetComponent<TransformComponent>();
		}

		m_Tranform = &GetComponent<TransformComponent>();

		if (!m_Player) { CONSOLE_ERROR("Player not found!"); }
	}

	void OnUpdate(DeltaTime deltaTime) override
	{
		if (!m_Player || !m_Tranform || !m_PlayerTranform) { return; }

		auto& playerPos = m_PlayerTranform->WorldPos;
		auto& cameraPos = m_Tranform->WorldPos;

		float distanceY = playerPos.y - cameraPos.y;
		float distanceX = playerPos.x - cameraPos.x;

		m_CameraSpeed = m_DefaultCameraSpeed;

		if (distanceY > 0.5f || distanceX > 0.5f || distanceY < -0.5f || distanceX < -0.5f)
		{
			m_CameraSpeed += 0.25f;
		}

		if (playerPos.x > cameraPos.x)
		{
			cameraPos.x += m_CameraSpeed * deltaTime;
		}

		if (playerPos.x < cameraPos.x)
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

	TransformComponent* m_PlayerTranform = nullptr;
	TransformComponent* m_Tranform = nullptr;

	float m_DefaultCameraSpeed = 0.35f;
	float m_CameraSpeed = 0.5f;

	Ref<Actor> m_Player;
};

class MainMenuScript : public ScriptableObject
{
public:

	std::shared_ptr<ScriptableObject> Instantiate() override
	{
		return std::make_shared<MainMenuScript>();
	}

	MainMenuScript() = default;

	void OnBeginPlay() override
	{
		if (HasComponent<CanvasComponent>())
		{
			auto& canvas = GetComponent<CanvasComponent>();

			const auto button = canvas.GetButton(0); // 0 is index inside canvas component
			if (button)
			{
				button->SetOnClickCallback(std::bind(&MainMenuScript::OnStartButtonPressed, this));
			}

			const auto button2 = canvas.GetButton(1);
			if (button2)
			{
				button2->SetOnClickCallback(std::bind(&MainMenuScript::OnQuitButtonPressed, this));
			}
		}
	}

	void OnStartButtonPressed()
	{
		EngineCommand::LoadScene(1); // 1 is scene index
	}

	void OnQuitButtonPressed()
	{
		EngineCommand::CloseApp();
	}
};