#include "EditorLayer.h"
#include "Core/Renderer/Renderer2D.h"
#include <imgui/imgui.h>

namespace SmolEngine
{
	static const char* s_Map =
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWW"
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWW"
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWW"
		"WWWWWWWWWDDDDDDDDDDDDWWWWWWW"
		"WWWWWWWWDDDDWWWWWWDDDDWWWWWW"
		"WWWWWDDDDWWWWWWWWWWDDDDWWWWW"
		"WWWWWWDDDDWWWWDWWWDDDDWWWWWW"
		"WWWWWWWWDDDDDDDDDDDDDWWWWWWW"
		"WWWWWWWWDDDDDDDDDDDDDWWWWWWW"
		"WWWWWWWWDDDDDDDDDDDWWWWWWWWW"
		"WWWWWWWWDDDDDDDDDDDDWWWWWWWW"
		"WWWWWWWDDDDDDDDDDDDDDWWWWWWW"
		"WWWWWWWWDDDDDDDDDDDDWWWWWWWW"
		"WWWWWWWWWWWDDDDWDWWWWWWWWWWW"
		"WWWWWWWWWWWWWDDWWWWWWWWWWWWW"
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWW"
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWW"
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWW"
		;

	static uint32_t s_MapWidth = 30, s_MapHeight = 18;

	void EditorLayer::OnAttach()
	{
		m_Texture = Texture2D::Create("Assets/Textures/Background.png");
		m_SheetTexture = Texture2D::Create("Assets/Textures/RPGpack_sheet_2X.png");

		m_HouseSubTexture = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 4.0f, 6.0f }, { 128.0f, 128.0f });
		m_FieldSubTexture = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 1.0f, 11.0f }, { 128.0f, 128.0f });

		m_TextureMap['D'] = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 6.0f, 11.0f }, { 128.0f, 128.0f });
		m_TextureMap['W'] = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 11.0f, 11.0f }, { 128.0f, 128.0f });


		m_Actor = m_Scene.CreateActor();
		auto& texture = m_Actor->AddComponent<Texture2DComponent>();
		texture.Texture = m_Texture;
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(DeltaTime deltaTime)
	{
		m_Scene.OnUpdate(deltaTime);
	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_Scene.GetEditorCamera()->OnEvent(event);
	}

	void EditorLayer::OnImGuiRender()
	{
		const GLubyte* renderer = glGetString(GL_RENDERER);

		static bool p_open = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File")) 
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Close"))
					Application::GetApplication().CloseApp();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Begin("Renderer2D Data");
		{
			ImGui::Text("Active GPU: %s", reinterpret_cast<char const*>(renderer));
			ImGui::Text("Draw Calls: %d", Renderer2D::GetData().DrawCalls);
			ImGui::Text("Quads Rendered: %d", Renderer2D::GetData().QuadCount);
			ImGui::Text("Total Vertices: %d", Renderer2D::GetData().GetTotalVertexCount());
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f});
		ImGui::Begin("Scene View");
		{
			auto& m_FrameBuffer = m_Scene.GetFrameBuffer();
			ImVec2 ViewPortSize = ImGui::GetContentRegionAvail();

			if (ViewPortSize.x != m_ViewPortSize.x || ViewPortSize.y != m_ViewPortSize.y)
			{
				m_ViewPortSize = { ViewPortSize.x, ViewPortSize.y };
				m_FrameBuffer->OnResize(m_ViewPortSize.x, m_ViewPortSize.y);
				m_Scene.GetEditorCamera()->OnResize(m_ViewPortSize.x, m_ViewPortSize.y);
			}
			uint32_t textureID = m_FrameBuffer->GetColorAttachmentID();
			ImGui::Image((void*)textureID, ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("Actor Tranform");
		{
			ImGui::InputFloat3("Transform", glm::value_ptr(m_Actor->GetComponent<TransfromComponent>().WorldPos));
			ImGui::InputFloat("Rotation", &m_Actor->GetComponent<TransfromComponent>().Rotation);
			ImGui::InputFloat2("Scale", glm::value_ptr(m_Actor->GetComponent<TransfromComponent>().Scale));
		}
		ImGui::End();


		ImGui::End();

	}
}