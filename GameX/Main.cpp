#include "Main.h"
#include "Core/EntryPoint.h"
#include "SmolEngineCore.h"
#include "Playground2D.h"


class Game : public SmolEngine::Application
{
public:

	//----------------------CLIENT-SIDE-INITIALIZATION----------------------//
	void ClientInit() override
	{
		auto& app = Application::GetApplication();
		//app.BindAction(SmolEngine::S_KEY_REPEAT, SmolEngine::S_KEY_S, BindSInput);
		//app.BindAction(SmolEngine::S_KEY_REPEAT, SmolEngine::S_KEY_D, BindDInput);

		CLIENT_INFO("Initialized successfully");
	}
	//----------------------CLIENT-SIDE-INITIALIZATION----------------------//
};

SmolEngine::Application* SmolEngine::CreateApp()
{
	return new Game;
}
