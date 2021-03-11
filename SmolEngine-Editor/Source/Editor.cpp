#include "Editor.h"

#include "EditorLayer.h"
#include "RaytracingTestLayer.h"
#include "DeferredRenderingTestLayer.h"
#include "Renderer3DTestLayer.h"

#include "ECS/Systems/ScriptingSystem.h"
#include "../../GameX/CppScriptingExamples.h"

namespace SmolEngine
{
	void Editor::OnEngineInitialized()
	{
		// Adds Scripts
		ScriptingSystem::AddNativeClass<CharMoveScript>("CharMoveScript");

		// Push Layers
		auto& engine = Engine::GetEngine();
		//engine.PushLayer(new Renderer3DTestLayer);
		//engine.PushLayer(new DeferredRenderingTest);
		engine.PushLayer(new EditorLayer);

		EDITOR_INFO("Initialized successfully");
	}


	Engine* CreateEngineContext()
	{
		return new Editor;
	}
}
