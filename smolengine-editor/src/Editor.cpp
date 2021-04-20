#include "Editor.h"
#include "EditorLayer.h"

#include "ECS/Systems/ScriptingSystem.h"

namespace SmolEngine
{
	Engine* CreateEngineContext()
	{
		return new Editor;
	}

	Editor::~Editor()
	{
		delete m_Camera;
	}

	void Editor::SetGraphicsContext(Frostium::GraphicsContextInitInfo* info)
	{
		info->pWindowCI->Height = 1080;
		info->pWindowCI->Width = 1920;
#ifdef EDITOR_DEBUG
		info->pWindowCI->Title = "SmolEngine Editor x64 - Vulkan - Debug";
#else
		info->pWindowCI->Title = "SmolEngine Editor x64 - Vulkan - Release";
#endif
		info->bTargetsSwapchain = false; // render imgui to the swap chain and later on render whole scene as imgui texture
	}

	void Editor::SetPhysicsContext(PhysicsContextCreateInfo* info)
	{

	}

	void Editor::SetLayers(LayerManager* layerManager)
	{
		Frostium::EditorCameraCreateInfo camCI = {};
		m_Camera = new Frostium::EditorCamera(&camCI);
		EditorLayer* editorLayer = new EditorLayer(m_Camera);

		layerManager->AddLayer(editorLayer);
	}

	void Editor::SetScripts(ScriptingSystem* scriptingSytem)
	{

	}
}
