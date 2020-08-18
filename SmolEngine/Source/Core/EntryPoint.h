#pragma once

#include "Core/SLog.h"
#include "Core/Application.h"

#ifdef PLATFORM_WIN

extern SmolEngine::Application* SmolEngine::CreateApp();


int main(int argc, char** argv)
{
	//Initializing Logging Tool
	SmolEngine::SLog::InitLog();

	SmolEngine::Application* app = SmolEngine::CreateApp();

	app->InitApp();
	CLIENT_INFO("App Initizlized!");
	delete app;

}

#endif

