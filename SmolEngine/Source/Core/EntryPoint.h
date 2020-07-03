#pragma once

#ifdef PLATFORM_WIN

extern SmolEngine::Application* SmolEngine::CreateApp();

int main(int args, char** argv)
{
	auto app = SmolEngine::CreateApp();
	app->StartApp();
	delete app;
}

#endif

