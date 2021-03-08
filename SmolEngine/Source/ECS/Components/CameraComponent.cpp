#include "stdafx.h"
#include "CameraComponent.h"

#include "Core/Engine.h"

namespace SmolEngine
{
	CameraComponent::CameraComponent()
	{
		AspectRatio = (float)Engine::GetEngine().GetWindowWidth() / (float)Engine::GetEngine().GetWindowHeight();
	}
}