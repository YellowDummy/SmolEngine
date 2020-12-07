#include "Editor.h"
#include "Core/EntryPoint.h"

#include "EditorLayer.h"
#include "VulkanTestLayer.h"


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

		// TEMP

#ifdef SMOLENGINE_OPENGL_IMPL

		app.PushLayer(new EditorLayer);

#else
		app.PushLayer(new VulkanTestLayer);
#endif

		EDITOR_INFO("Initialized successfully");
	}


	Application* CreateApp()
	{
		return new Editor;
	}
}
