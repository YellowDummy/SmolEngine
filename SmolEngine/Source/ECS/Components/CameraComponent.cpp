#include "stdafx.h"
#include "CameraComponent.h"

#include "Core/Application.h"

namespace SmolEngine
{
	CameraComponent::CameraComponent()
	{
		AspectRatio = (float)Application::GetApplication().GetWindowWidth() / (float)Application::GetApplication().GetWindowHeight();
	}
}