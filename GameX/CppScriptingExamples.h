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

	}

	void OnUpdate(DeltaTime deltaTime) override
	{

	}

	void OnCollisionContact(Actor* actor) override
	{

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

	}

	void OnUpdate(DeltaTime deltaTime) override
	{
		
	}

	void OnDestroy() override  {}

private:

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