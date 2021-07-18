#pragma once

#include "SmolEngineCore.h"

namespace SmolEngine
{
	class EditorCamera;

	class Editor : public Engine
	{
	public:

		~Editor();

		void OnGraphicsModuleCreation(GraphicsContextInitInfo* info) override;
		void OnPhysicsModuleCreation(PhysicsModuleCreateInfo* info) override;
		void OnLayerModuleCreation(LayerManager* layerManager) override;
		void OnScriptModuleCreation(ScriptingSystem* scriptingSytem) override;
		void OnWorldAdminModuleCreation(WorldAdminStateSComponent* state) override;

	private:

		EditorCamera* m_Camera = nullptr;
	};

}

