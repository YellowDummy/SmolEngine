#pragma once

#include "SmolEngineCore.h"

namespace SmolEngine
{
	class EditorCamera;

	class Editor : public Engine
	{
	public:

		~Editor();

		void SetGraphicsContext(GraphicsContextInitInfo* info) override;
		void SetPhysics2DContext(Physics2DContextCreateInfo* info)override;
		void SetLayers(LayerManager* layerManager) override;
		void SetScripts(ScriptingSystem* scriptingSytem) override;
		void SetWorldAdminState(WorldAdminStateSComponent* state) override;

	private:

		EditorCamera* m_Camera = nullptr;
	};

}

