#include "stdafx.h"
#include "Core/EntryPoint.h"

#ifdef PLATFORM_WIN

extern SmolEngine::Engine* SmolEngine::CreateEngineContext();

int main(int argc, char** argv)
{
	SmolEngine::Engine* app = SmolEngine::CreateEngineContext();
	app->Init();
}

#endif