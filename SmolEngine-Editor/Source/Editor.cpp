#include "Editor.h"

#include "EditorLayer.h"
#include "RaytracingTestLayer.h"
#include "DeferredRenderingTestLayer.h"
#include "Renderer3DTestLayer.h"

namespace SmolEngine
{
	void Editor::OnEngineInitialized()
	{
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
