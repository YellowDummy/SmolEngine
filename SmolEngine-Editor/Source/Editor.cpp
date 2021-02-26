#include "Editor.h"
#include "Core/EntryPoint.h"

#include "EditorLayer.h"
#include "VulkanTestLayer.h"
#include "RaytracingTestLayer.h"
#include "DeferredRenderingTestLayer.h"
#include "Renderer3DTestLayer.h"

namespace SmolEngine
{
	Editor::Editor()
	{

	}

	Editor::~Editor()
	{

	}

	void Editor::ClientInit()
	{
		auto& app = Application::GetApplication();

#ifdef SMOLENGINE_OPENGL_IMPL
		app.PushLayer(new EditorLayer);
#else
		//app.PushLayer(new Renderer3DTestLayer);
		//app.PushLayer(new DeferredRenderingTest);
#endif
		app.PushLayer(new EditorLayer);
		EDITOR_INFO("Initialized successfully");
	}

	Application* CreateApp()
	{
		return new Editor;
	}
}
