#include "stdafx.h"
#include "Application.h"
#include "SLog.h"

#include "Window.h"

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
		NATIVE_INFO("Starting SmolEngine...");

		Window win(std::string("SmolEngine Demo - v0.1"), 720, 1280);

		NATIVE_INFO("SmolEngine launched successfully");
	}

	void Application::StartApp()
	{
		EngineInit();

		while (true)
		{

		}
	}

}
