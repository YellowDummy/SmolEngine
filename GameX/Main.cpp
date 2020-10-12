#include "Main.h"
#include "Core/EntryPoint.h"
#include "SmolEngineCore.h"
#include "GameLayer.h"


class Game : public SmolEngine::Application
{
public:

	void ClientInit() override
	{
		auto& app = Application::GetApplication();
		app.PushLayer(new GameLayer());

		CLIENT_INFO("Initialized successfully");
	}
};

SmolEngine::Application* SmolEngine::CreateApp()
{
	return new Game;
}
