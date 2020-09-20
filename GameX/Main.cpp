#include "Main.h"
#include "Core/EntryPoint.h"
#include "SmolEngineCore.h"


class Game : public SmolEngine::Application
{
public:

	void ClientInit() override
	{
		auto& app = Application::GetApplication();
		CLIENT_INFO("Initialized successfully");
	}
};

SmolEngine::Application* SmolEngine::CreateApp()
{
	return new Game;
}
