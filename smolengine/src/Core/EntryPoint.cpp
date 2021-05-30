#include "stdafx.h"
#include "Core/EntryPoint.h"

#ifdef PLATFORM_WIN

extern SmolEngine::Engine* CreateEngineContext();

int main(int argc, char** argv)
{
	SmolEngine::Engine* app = CreateEngineContext();
	app->Init();
}

#endif