#include "stdafx.h"
#include "Application.h"
#include "SLog.h"

namespace SmolEngine 
{
	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::EngineInit()
	{
		SmolEngine::SLog::InitLog();
	}

	void Application::StartApp()
	{
		EngineInit();
		NATIVE_INFO("Starting SmolEngine...");

		while (true)
		{

		}
	}

}
