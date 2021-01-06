#pragma once

// 06.01.2021
//
// Scripting in C++

#include <SmolEngineCore.h>

using namespace SmolEngine;

class CharMovementScript: public BehaviourPrimitive
{
public:

	CharMovementScript()
	{
		OUT_FLOAT("Line xPos", &line_x);
		OUT_FLOAT("Line yPos", &line_y);

		OUT_INT("Player Speed", &player_speed);
	}

	void OnBegin()
	{
		m_Body2D = GetComponent<Body2DComponent>();
	}

	void OnProcess(DeltaTime deltaTime)
	{
		if (!m_Body2D)
			return;

		if (Input::IsKeyPressed(KeyCode::Left))
			Physics2DSystem::AddForce(m_Body2D, { -player_speed, 0 });
		
		if (Input::IsKeyPressed(KeyCode::Right))
			Physics2DSystem::AddForce(m_Body2D, { player_speed, 0 });

		if (Input::IsKeyPressed(KeyCode::Up))
			Physics2DSystem::AddForce(m_Body2D, { 0, player_speed + 15 });
	}

	void OnDestroy() 
	{

	}

	/// Optional Methods

	void OnCollisionContact(Actor* another, bool isTrigger) override
	{

	}

	void OnCollisionExit(Actor* another, bool isTrigger) override
	{

	}

	void OnDebugDraw() override
	{
		RendererSystem::SubmitDebugLine({ 0, 0, 0 }, { line_x, line_y, 0 });
	}

private:

	Body2DComponent* m_Body2D = nullptr;

	float line_x = 10.0f;
	float line_y = 10.0f;
	int player_speed = 10;
};

class CameraMovementScript : public BehaviourPrimitive
{
public:

	CameraMovementScript()
	{
		OUT_FLOAT("SpeedCamera", &player_speed);
		OUT_INT("Speed2Camera", &player_speed2);
	}

	void OnBegin()
	{

	}

	void OnProcess(DeltaTime deltaTime)
	{

	}

	void OnDestroy() 
	{

	}

private:

	float player_speed = 140.0f;
	int player_speed2 = 105;
};