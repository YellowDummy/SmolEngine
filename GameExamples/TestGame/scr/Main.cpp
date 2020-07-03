#include "SmolEngineCore.h"

class Game : public SmolEngine::Application
{
public:

	Game() 
	{

	}

	~Game() 
	{

	}
};

SmolEngine::Application* SmolEngine::CreateApp()
{
	return new Game;
}