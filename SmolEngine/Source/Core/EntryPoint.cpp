#include "stdafx.h"
#include "EntryPoint.h"

#ifdef PLATFORM_WIN

extern SmolEngine::Engine* SmolEngine::CreateEngineContext();

int main(int argc, char** argv)
{
	SmolEngine::SLog::InitLog();
	SmolEngine::Engine* app = SmolEngine::CreateEngineContext();
	app->Init();
}

#endif