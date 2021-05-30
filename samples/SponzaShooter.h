#include "SmolEngineCore.h"

using namespace SmolEngine;

class Game: public Engine
{
public:

	void SetGraphicsContext(GraphicsContextInitInfo* info) override;
	void SetPhysics2DContext(Physics2DContextCreateInfo* info)override;
	void SetLayers(LayerManager* layerManager) override;
	void SetScripts(ScriptingSystem* scriptingSytem) override;
	void OnInitializationComplete(WorldAdmin* admin) override;
};