#include "SponzaShooter.h"
#include "Scripts.h"

Engine* CreateEngineContext()
{
	return new Game();
}

void Game::OnInitializationComplete(WorldAdmin* admin)
{
	admin->LoadSceneRuntime("scenes/SponzaGameV3.s_scene");
}

void Game::OnGraphicsModuleCreation(GraphicsContextInitInfo* info)
{

}

void Game::OnPhysicsModuleCreation(PhysicsModuleCreateInfo* info)
{

}

void Game::OnLayerModuleCreation(LayerManager* layerManager)
{

}

void Game::OnScriptModuleCreation(ScriptingSystem* scriptingSytem)
{

}

void Game::OnWorldAdminModuleCreation(WorldAdminStateSComponent* state)
{

}
