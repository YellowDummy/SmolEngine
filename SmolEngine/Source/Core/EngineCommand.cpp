#include "stdafx.h"
#include "EngineCommand.h"

#include "Core/SLog.h"
#include "Core/ECS/Scene.h"
#include "Core/Application.h"

namespace SmolEngine
{
	void EngineCommand::LoadScene(uint32_t index)
	{
		const auto scene = Scene::GetScene();

		scene->OnEndPlay();

		if (!scene->LoadSceneRuntime(index))
		{
			NATIVE_ERROR("Failed to load the scene, index: {}", index);
		}
	}

	void EngineCommand::CloseApp()
	{
		Application::GetApplication().CloseApp();
	}
}