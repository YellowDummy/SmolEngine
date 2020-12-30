#pragma once

// 29.12.2020
//
// Scripting in C++

#include <SmolEngineCore.h>

using namespace SmolEngine;

class CharMovementScript: public BehaviourPrimitive
{
public:

	CharMovementScript()
	{
		OUT_FLOAT("Speed", &player_speed);
		OUT_INT("Speed2", &player_speed2);
	}

	void OnBegin()
	{

	}

	void OnProcess(DeltaTime deltaTime)
	{

	}

	void OnCollisionContact(Actor* another, bool isTrigger)
	{

	}

	void OnCollisionExit(Actor* another, bool isTrigger)
	{

	}

	void OnDestroy() {}

private:

	float player_speed = 10.0f;
	int player_speed2 = 10.0f;
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

	void OnCollisionContact(Actor* another, bool isTrigger)
	{

	}

	void OnCollisionExit(Actor* another, bool isTrigger)
	{

	}

	void OnDestroy() {}

private:

	float player_speed = 140.0f;
	int player_speed2 = 105.0f;
};