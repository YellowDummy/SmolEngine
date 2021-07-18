#include "Editor.h"
#include "EditorLayer.h"
#include "Scripts/BasePlayerScript.h"
#include "Scripts/BallSpawner.h"
#include "ECS/Systems/ScriptingSystem.h"

SmolEngine::Engine* CreateEngineContext()
{
	return new SmolEngine::Editor;
}

namespace SmolEngine
{
	Editor::~Editor()
	{
		delete m_Camera;
	}

	void Editor::OnGraphicsModuleCreation(GraphicsContextInitInfo* info)
	{
		info->pWindowCI->Height = 1080;
		info->pWindowCI->Width = 1920;
		info->Flags |= Features_Renderer_Debug_Flags;
#ifdef EDITOR_DEBUG
		info->pWindowCI->Title = "SmolEngine Editor - Debug (Vulkan x64)";
#else
		info->pWindowCI->Title = "SmolEngine Editor - Release (Vulkan x64)";
#endif
		info->bAutoResize = false;
		info->bTargetsSwapchain = false; // render imgui to the swap chain and later on render whole scene as imgui texture
	}

	void Editor::OnPhysicsModuleCreation(PhysicsModuleCreateInfo* info)
	{
	}

	void Editor::OnLayerModuleCreation(LayerManager* layerManager)
	{
		EditorCameraCreateInfo camCI = {};
		camCI.WorldPos = { 0, 5, 0 };
		m_Camera = new EditorCamera(&camCI);
		EditorLayer* editorLayer = new EditorLayer(m_Camera);

		layerManager->AddLayer(editorLayer);
	}

	void Editor::OnScriptModuleCreation(ScriptingSystem* scriptingSytem)
	{
		scriptingSytem->AddNativeClass<BasePlayerScript>("Base Player Script");
		scriptingSytem->AddNativeClass<BallSpawner>("Ball Spawner");
	}

	void Editor::OnWorldAdminModuleCreation(WorldAdminStateSComponent* state)
	{
		state->m_LevelEditorActive = true;
	}
}
