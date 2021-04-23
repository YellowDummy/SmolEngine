#pragma once

#include "SmolEngineCore.h"

class Frostium::EditorCamera;

namespace SmolEngine
{
	class Editor : public Engine
	{
	public:

		~Editor();

		void SetGraphicsContext(Frostium::GraphicsContextInitInfo* info) override;
		void SetPhysics2DContext(Physics2DContextCreateInfo* info)override;
		void SetLayers(LayerManager* layerManager) override;
		void SetScripts(ScriptingSystem* scriptingSytem) override;

	private:

		Frostium::EditorCamera* m_Camera = nullptr;
	};

}

