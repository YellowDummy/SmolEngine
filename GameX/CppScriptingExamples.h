#pragma once

// 09.03.2021
//
// Scripting in C++

#include <SmolEngineCore.h>

using namespace SmolEngine;

class CharMoveScript : public BehaviourPrimitive // Note: inheritance is required
{
public:

	CharMoveScript() // Default c-tor is required
	{
		// Exposing properties to level-editor
		// Currently supported only int, float and std::string
		// Valid only inside default c-tor
		CreateValue<float>("Speed");
		CreateValue<std::string>("Name");
	}

	// All functions below must be implemented
	// They will be called using runtime reflection

	void OnBegin()
	{
		speed = GetValue<float>("Speed");
		name = GetValue<std::string>("Name");

		NATIVE_ERROR("Speed is {}", *speed);
		NATIVE_ERROR("Name is {}", *name);
	}

	void OnProcess(DeltaTime deltaTime)
	{

	}

	void OnDestroy() {}
	void OnCollisionContact(Actor * another, bool isTrigger) {};
	void OnCollisionExit(Actor* another, bool isTrigger) {};
	void OnDebugDraw() {};

private:

	float* speed = nullptr;
	std::string* name = nullptr;
};