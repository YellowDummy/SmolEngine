#pragma once

// 29.10.2020
//
// Scripting using C++

#include <SmolEngineCore.h>
#include <functional>

using namespace SmolEngine;

/// Type: Physics

class CharMovementSystem : public PhysicsTupleBehaviour
{
public:

	CharMovementSystem()
	{
		OUT_FLOAT("Player Speed", &m_Speed);
		OUT_STRING("Player Name", &m_PlayerName);
	}

	// Main

	void OnBegin(PhysicsBaseTuple& tuple)
	{
		CONSOLE_WARN("Actor name is: " + tuple.GetInfo().Name);

		CONSOLE_WARN("m_PlayerName is: " + m_PlayerName);
	}

	void OnProcess(DeltaTime deltTime, PhysicsBaseTuple& tuple)
	{
		if (Input::IsKeyPressed(KeyCode::E))
		{
			AddForce(tuple, { m_Speed, 0.0f });
		}

		if (Input::IsKeyPressed(KeyCode::Q))
		{
			AddForce(tuple, { -m_Speed, 0.0f });
		}

		if (Input::IsKeyPressed(KeyCode::Space))
		{
			AddForce(tuple, { 0.0f, m_Speed });
		}
	}

	// Callbacks

	void OnCollisionContact(Actor* actor, bool isTrigger) { CONSOLE_ERROR("CollisonContact: " + actor->GetName()); }

	void OnCollisionExit(Actor* actor, bool isTrigger) { CONSOLE_ERROR("CollisonExit: " + actor->GetName()); }

	void OnDestroy() {}

private:

	std::string m_PlayerName = "Player_0";

	float m_Speed = 25.0f;
};

/// Type: Camera

class CameraMovementSystem : public CameraTupleBehaviour
{
public:

	void OnBegin(CameraBaseTuple& tuple)
	{
		CONSOLE_ERROR("Camera name is: " + tuple.GetInfo().Name);
	}

	void OnProcess(DeltaTime deltTime, CameraBaseTuple& tuple) {}

	void OnDestroy() {}

};

/// Type: Camera

class MainMenuSystem : public CameraTupleBehaviour
{
public:

	void OnBegin(CameraBaseTuple& tuple) {}

	void OnProcess(DeltaTime deltTime, CameraBaseTuple& tuple) {}

	void OnDestroy() {}
};

/// Type: Base

class DummyActorSystem : public BaseTupleBehaviour
{
public:

	void OnBegin(DefaultBaseTuple& tuple) {}

	void OnProcess(DeltaTime deltTime, DefaultBaseTuple& tuple) {}

	void OnDestroy() {}
};