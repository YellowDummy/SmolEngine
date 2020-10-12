#include "stdafx.h"
#include "EntryPoint.h"

#ifdef PLATFORM_WIN


extern SmolEngine::Application* SmolEngine::CreateApp();



int main(int argc, char** argv)
{
	//Initializing Logging Tool
	SmolEngine::SLog::InitLog();

	SmolEngine::Application* app = SmolEngine::CreateApp();
	app->InitApp();


	delete app;
}

#endif