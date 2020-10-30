#include "stdafx.h"
#include "SystemRegistry.h"
#include "rttr/registration.h"

#include "../../GameX/CppScriptingExamples.h"

namespace SmolEngine
{
	SystemRegistry* SystemRegistry::Instance = nullptr;

	/// Add Your Systems Here

	RTTR_REGISTRATION
	{
		// Physics Systems

		SystemRegistry::AddPhysicsSystem<CharMovementSystem>("CharMovementSystem");

	    // Camera Systems

	    SystemRegistry::AddCameraSystem<CameraMovementSystem>("CameraMovementSystem");
		SystemRegistry::AddCameraSystem<MainMenuSystem>("MainMenuSystem");

		// Base Systems

		SystemRegistry::AddDefaultSystem<DummyActorSystem>("DummyActorSystem");
	}
}