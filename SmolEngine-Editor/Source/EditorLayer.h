#pragma once
#include "SmolEngineCore.h"
#include "Core/Layer.h"
#include "Core/ImGui/EditorConsole.h"
#include "Core/ECS/Components.h"
#include "Core/ECS/Scene.h"
#include "Core/ECS/DefaultSystems.h"

#include <imgui/imgui.h>
#include "Core/ImGui/FileBrowser/imfilebrowser.h"

namespace SmolEngine
{
	static Ref<EditorConsole> s_EditorConsole;

#define CONSOLE_INFO(...) s_EditorConsole->AddMessageInfo(__VA_ARGS__)
#define CONSOLE_WARN(...) s_EditorConsole->AddMessageWarn(__VA_ARGS__)
#define CONSOLE_ERROR(...) s_EditorConsole->AddMessageError(__VA_ARGS__)


	enum class ComponentItem : char
	{
		None = 0, 
		Tetxure2D, JinxScript, Rigidbody2D, AnimationContoller, ParticleSystem
	};

	enum class EditorViewportFlags : uint16_t
	{
		None = 0,
		is_SceneView
	};

	enum class FileBrowserFlags : uint32_t
	{
		None = 0, ScriptPath, Texture2dPath
	};

	class EditorLayer: public Layer
	{
	public:
		EditorLayer()
			:Layer("EditorLayer")
		{

		}

		~EditorLayer() {}

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(DeltaTime deltaTime) override;
		void OnEvent(Event& event) override;
		void OnImGuiRender() override;

	private:
		void ResetFileBrowser();

	private:

		glm::vec2 m_ViewPortSize = {0.0f, 0.0f};
		std::string m_FilePath = "";
		Ref<Scene> m_Scene;

		//TEMP 
		Ref<Actor> m_Actor = nullptr;
		Ref<Actor> m_CameraActor = nullptr;
		Ref<Actor> m_SelectedActor = nullptr;
		Ref<ImGui::FileBrowser> m_FileBrowser = nullptr;
		Ref<Texture2D> m_Texture = nullptr;
		Ref<Texture2D> m_SheetTexture = nullptr;
		Ref<SubTexture2D> m_HouseSubTexture = nullptr;
		Ref<SubTexture2D> m_FieldSubTexture = nullptr;

		FileBrowserFlags m_FileBrowserState = FileBrowserFlags::None;
		EditorViewportFlags m_EditorViewportFlags = EditorViewportFlags::None;
		bool PlayMode = false;
	};
}

