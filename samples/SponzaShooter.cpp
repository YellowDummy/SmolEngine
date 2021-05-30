#include "SponzaShooter.h"
#include "Scripts.h"

Engine* CreateEngineContext()
{
	return new Game();
}

void Game::SetGraphicsContext(GraphicsContextInitInfo* info)
{
	info->pWindowCI->Height = 1080;
	info->pWindowCI->Width = 1920;
	info->pWindowCI->Title = "Sponza Shooter x64 *Vulkan)";
	info->pWindowCI->bFullscreen = true;
}

void Game::SetPhysics2DContext(Physics2DContextCreateInfo* info)
{

}

void Game::SetLayers(LayerManager* layerManager)
{

}

void Game::SetScripts(ScriptingSystem* scriptingSytem)
{
	scriptingSytem->AddNativeClass<BasePlayerScript>("Base Player Script");
}

void Game::OnInitializationComplete(WorldAdmin* admin)
{
	admin->LoadSceneRuntime("scenes/SponzaGameV3.s_scene");
}
