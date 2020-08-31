#include "Editor.h"
#include "Core/EntryPoint.h"

#include "EditorLayer.h"


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
		app.PushLayer(new EditorLayer);

		EDITOR_INFO("Initialized successfully");
	}


	Application* CreateApp()
	{
		return new Editor;
	}
}
