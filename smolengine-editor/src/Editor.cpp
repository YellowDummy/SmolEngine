#include "Editor.h"
#include "EditorLayer.h"
#include "Scripts/BasePlayerScript.h"

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

	void Editor::SetGraphicsContext(GraphicsContextInitInfo* info)
	{
		info->pWindowCI->Height = 1080;
		info->pWindowCI->Width = 1920;
		info->Flags |= Features_Renderer_Debug_Flags;
#ifdef EDITOR_DEBUG
		info->pWindowCI->Title = "SmolEngine Editor x64 - Vulkan - Debug";
#else
		info->pWindowCI->Title = "SmolEngine Editor x64 - Vulkan - Release";
#endif
		info->bAutoResize = false;
		info->bTargetsSwapchain = false; // render imgui to the swap chain and later on render whole scene as imgui texture
	}

	void Editor::SetPhysics2DContext(Physics2DContextCreateInfo* info)
	{

	}

	void Editor::SetLayers(LayerManager* layerManager)
	{
		EditorCameraCreateInfo camCI = {};
		camCI.WorldPos = { 0, 5, 0 };
		m_Camera = new EditorCamera(&camCI);
		EditorLayer* editorLayer = new EditorLayer(m_Camera);

		layerManager->AddLayer(editorLayer);
	}

	void Editor::SetScripts(ScriptingSystem* scriptingSytem)
	{
		scriptingSytem->AddNativeClass<BasePlayerScript>("Base Player Script");
	}
}
