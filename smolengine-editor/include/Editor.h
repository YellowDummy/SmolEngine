#pragma once

#include "SmolEngineCore.h"

namespace SmolEngine
{
	class Editor : public Engine
	{
	public:

		void SetGraphicsContext(Frostium::GraphicsContextInitInfo* info) override;
		void SetPhysicsContext(PhysicsContextCreateInfo* info)override;
		void SetLayers(LayerManager* layerManager) override;
		void SetScripts(ScriptingSystem* scriptingSytem) override;
	};

}

