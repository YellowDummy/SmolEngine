#include "Main.h"
#include "SmolEngineCore.h"
#include "GameLayer.h"

class Game : public SmolEngine::Engine
{
public:

	void OnEngineInitialized() override
	{
		auto& app = Engine::GetEngine();
		app.PushLayer(new GameLayer());

		CLIENT_INFO("Initialized successfully");
	}
};

SmolEngine::Engine* SmolEngine::CreateEngineContext()
{
	return new Game;
}
