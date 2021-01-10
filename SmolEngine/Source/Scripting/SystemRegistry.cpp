#include "stdafx.h"
#include "SystemRegistry.h"
#include "rttr/registration.h"

#include "../../GameX/CppScriptingExamples.h"

namespace SmolEngine
{
	SystemRegistry* SystemRegistry::Instance = nullptr;

	/// Add Your C++ Scripts Here

	RTTR_REGISTRATION
	{
		SystemRegistry::AddScript<CharMovementScript>("CharMovement");
	    SystemRegistry::AddScript<CameraMovementScript>("CameraMovement");
	}
}