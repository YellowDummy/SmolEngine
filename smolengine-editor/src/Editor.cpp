#include "Editor.h"
#include "EditorLayer.h"

#include "ECS/Systems/ScriptingSystem.h"

namespace SmolEngine
{
	Engine* CreateEngineContext()
	{
		return new Editor;
	}

	void Editor::SetGraphicsContext(Frostium::GraphicsContextInitInfo* info)
	{
		info->pWindowCI->Height = 1080;
		info->pWindowCI->Width = 1920;
	}

	void Editor::SetPhysicsContext(PhysicsContextCreateInfo* info)
	{

	}

	void Editor::SetLayers(LayerManager* layerManager)
	{
		layerManager->AddLayer(new EditorLayer);
	}

	void Editor::SetScripts(ScriptingSystem* scriptingSytem)
	{

	}
}
