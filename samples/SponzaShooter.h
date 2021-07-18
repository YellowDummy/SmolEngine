#include "SmolEngineCore.h"

using namespace SmolEngine;

class Game: public Engine
{
public:

	void OnGraphicsModuleCreation(GraphicsContextInitInfo* info) override;
	void OnPhysicsModuleCreation(PhysicsModuleCreateInfo* info) override;
	void OnLayerModuleCreation(LayerManager* layerManager) override;
	void OnScriptModuleCreation(ScriptingSystem* scriptingSytem) override;
	void OnWorldAdminModuleCreation(WorldAdminStateSComponent* state) override;
	void OnInitializationComplete(WorldAdmin* admin) override;
};