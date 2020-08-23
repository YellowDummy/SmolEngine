#include "stdafx.h"
#include "EditorLayer.h"
#include "Core/Scripting/Native.h"
#include "Core/Renderer/Renderer2D.h"
#include "Core/Scripting/Jinx.h"

#include <imgui/imgui.h>

namespace SmolEngine
{

	enum class ComponentItem: char
	{
		None = 0, Tetxure2D, JinxScript, Camera, AnimationContoller, ParticleSystem
	};

	//Example Of Native C++ Script
	class CharMovementScript: public ScriptableObject
	{
	public:

		CharMovementScript(Ref<Actor> actor)
			:ScriptableObject(actor)
		{
		}

		void Start() override 
		{
		}

		void OnUpdate(DeltaTime deltaTime) override
		{
			f1 += 3.0f * deltaTime;

			if (!switchState)
			{
				f2 += 1.0f * deltaTime;

				if (f2 > 1.0f)
				{
					switchState = true;
				}
			}
			else
			{
				if (f2 > -0.8f)
				{
					f2 -= 1.0f * deltaTime;
				}
				else
				{
					switchState = false;
				}
			}

			GetComponent<TransfromComponent>().WorldPos = glm::vec3(0, f2, 1);
			GetComponent<TransfromComponent>().Rotation = f1;
		}

		void OnDestroy() override {}

		Jinx::ScriptPtr m_Script;
		Jinx::RuntimePtr m_Runtime;

	private:
		bool switchState = false;
		float f1 = 0, f2 = -0.8f;
	};

	void EditorLayer::OnAttach()
	{
		m_FileBrowser = std::make_shared<ImGui::FileBrowser>();

		m_Texture = Texture2D::Create("Assets/Textures/Background.png");
		m_SheetTexture = Texture2D::Create("Assets/Textures/RPGpack_sheet_2X.png");

		m_HouseSubTexture = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 4.0f, 6.0f }, { 128.0f, 128.0f });
		m_FieldSubTexture = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 1.0f, 11.0f }, { 128.0f, 128.0f });
		m_TextureMap['D'] = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 6.0f, 11.0f }, { 128.0f, 128.0f });
		m_TextureMap['W'] = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 11.0f, 11.0f }, { 128.0f, 128.0f });

		m_Actor = m_Scene.CreateActor("Actor_1");
		m_Actor->AddComponent<Texture2DComponent>(m_Texture);
		m_Actor->AddComponent<ScriptComponent>().SetScript<CharMovementScript>(m_Actor);

		m_CameraActor = m_Scene.CreateActor("Camera");
		m_CameraActor->AddComponent<CameraComponent>();


		auto nameActor = m_Scene.FindActorByName("Actor_1");
		auto tagActor = m_Scene.FindActorByTag("Default");

		// Initialize Lua
		//lua_State* L = luaL_newstate();
		//luaL_openlibs(L);

		//JinxScript script(m_Actor, m_Scene.GetJinxRuntime(), std::string("../GameX/Assets/JinxScripts/Example.jinx"));
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
		m_Scene.OnEvent(event);
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
				{
					Application::GetApplication().CloseApp();
				}

				if (ImGui::MenuItem("Save Scene"))
				{

				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Settings"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Build"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Build & Run"))
			{
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		if(ImGui::Begin("Renderer2D: Stats"))
		{
			ImGui::Text("Active GPU: %s", reinterpret_cast<char const*>(renderer));
			ImGui::Text("Draw Calls: %d", Renderer2D::GetData().DrawCalls);
			ImGui::Text("Quads Rendered: %d", Renderer2D::GetData().QuadCount);
			ImGui::Text("Total Vertices: %d", Renderer2D::GetData().GetTotalVertexCount());
			ImGui::End();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f});
		if(ImGui::Begin("Scene View"))
		{
			auto& m_FrameBuffer = m_Scene.GetFrameBuffer();
			ImVec2 ViewPortSize = ImGui::GetContentRegionAvail();

			if (ViewPortSize.x != m_ViewPortSize.x || ViewPortSize.y != m_ViewPortSize.y)
			{
				m_ViewPortSize = { ViewPortSize.x, ViewPortSize.y };
				m_FrameBuffer->OnResize(m_ViewPortSize.x, m_ViewPortSize.y);
				m_Scene.OnEditorResize(m_ViewPortSize.x, m_ViewPortSize.y);
			}
			uint32_t textureID = m_FrameBuffer->GetColorAttachmentID();
			ImGui::Image((void*)textureID, ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::End();
		}
		ImGui::PopStyleVar();

		if(ImGui::Begin("Tools"));
		{
			if (ImGui::TreeNode("Create Actor"))
			{
				static char name[128] = "Default Actor";
				ImGui::InputText("Name", name, IM_ARRAYSIZE(name));

				static char tag[128] = "Default";
				ImGui::InputText("Tag", tag, IM_ARRAYSIZE(tag));

				if (ImGui::Button("Add", ImVec2{ 100, 25 }))
				{
					m_Scene.CreateActor(name, tag);
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Create System"))
			{
				ImGui::TreePop();
			}

			ImGui::End();
		}

		if(ImGui::Begin("Hierarchy"));
		{
			for (auto& actor: m_Scene.GetActorPool())
			{

				if (actor->IsDisabled) { continue; }

				if (ImGui::TreeNode(actor->GetName().c_str()))
				{
					if (ImGui::IsItemHovered())
					{
						if (Input::IsMouseButtonPressed(MouseCode::ButtonRight))
						{

						}

					}

					if (ImGui::TreeNode("Components"))
					{

						if (ImGui::TreeNode("Transform"))
						{
							ImGui::InputFloat3("Transform", glm::value_ptr(actor->GetComponent<TransfromComponent>().WorldPos));
							ImGui::InputFloat("Rotation", &actor->GetComponent<TransfromComponent>().Rotation);
							ImGui::InputFloat2("Scale", glm::value_ptr(actor->GetComponent<TransfromComponent>().Scale));
							ImGui::TreePop();
						}

						if (actor->HasComponent<Texture2DComponent>())
						{
							if (ImGui::TreeNode("Texture2D"))
							{
								ImGui::Checkbox("Texture2D Enabled", &actor->GetComponent<Texture2DComponent>().Enabled);
								ImGui::ColorEdit3("Color", glm::value_ptr(actor->GetComponent<Texture2DComponent>().Color));
								ImGui::TreePop();
							}
						}

						if (actor->HasComponent<JinxScriptComponent>())
						{
							if (ImGui::TreeNode("Jinx Script"))
							{
								ImGui::Checkbox("JinxScript Enabled", &actor->GetComponent<JinxScriptComponent>().Enabled);
								ImGui::TreePop();
							}
						}

						if (actor->HasComponent<ScriptComponent>())
						{
							if (ImGui::TreeNode("C++ Script"))
							{
								ImGui::Checkbox("C++ Script Enabled", &actor->GetComponent<ScriptComponent>().Enabled);
								ImGui::TreePop();
							}
						}

						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Actions"))
					{
						if (ImGui::TreeNode("Add/Delete Component"))
						{
							static int current_item = 0;
							ImGui::Combo("Component", &current_item, "None\0Texture2D\0JinxScript\0AnimationContoller\0ParticleSystem\0\0");

							if (ImGui::Button("Add", ImVec2{ 60, 25 }))
							{
								switch (current_item)
								{
								case (uint32_t)ComponentItem::None:
								{
									break;
								}
								case (uint32_t)ComponentItem::Tetxure2D:
								{
									if (!actor->HasComponent<Texture2DComponent>())
									{
										actor->AddComponent<Texture2DComponent>();
										break;
									}

									EDITOR_WARN("Actor <{}> already has Tetxure2D component.", actor->GetName());
									break;
								}
								case (uint32_t)ComponentItem::JinxScript:
								{
									if (!actor->HasComponent<JinxScriptComponent>())
									{
										m_SelectedActor = actor;
										m_FileBrowser->Open();
										break;
									}

									EDITOR_WARN("Actor <{}> already has Script component.", actor->GetName());
									break;
								}
								default:
									break;
								}

								current_item = 0;
							}

							if (ImGui::Button("Delete", ImVec2{ 60, 25 }))
							{
								switch (current_item)
								{
								case (uint32_t)ComponentItem::None:
								{
									break;
								}
								case (uint32_t)ComponentItem::Tetxure2D:
								{
									if (actor->HasComponent<Texture2DComponent>())
									{
										actor->DeleteComponent<Texture2DComponent>();
										break;
									}

									EDITOR_WARN("Actor <{}> does not have Tetxure2D component.", actor->GetName());
									break;
								}
								case (uint32_t)ComponentItem::JinxScript:
								{
									if (actor->HasComponent<JinxScriptComponent>())
									{
										actor->DeleteComponent<JinxScriptComponent>();
										break;
									}

									EDITOR_WARN("Actor <{}> does not have Script component.", actor->GetName());
									break;
								}
								default:
									break;
								}

								current_item = 0;
							}

							ImGui::TreePop();
						}

						if (ImGui::TreeNode("Rename Actor"))
						{
							static char name[128] = "Default Actor";
							ImGui::InputText("Name", name, IM_ARRAYSIZE(name));

							if (ImGui::Button("OK", ImVec2{ 60, 25 }))
							{
								actor->GetName() = std::string(name);
							}

							ImGui::TreePop();
						}

						if (ImGui::TreeNode("Save as Blueprint"))
						{
							if (ImGui::Button("Save", ImVec2{ 60, 25 }))
							{
								//!TODO: Serialize Actor Class
							}

							ImGui::TreePop();
						}
						
						if (ImGui::TreeNode("Delete Actor"))
						{
							if (ImGui::Button("Delete", ImVec2{ 60, 25 }))
							{
								//TODO: Fix DeleteActorImmediately method
								m_Scene.DeleteActor(actor);
							}

							ImGui::TreePop();
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}

			ImGui::End();
		}

		ImGui::End();

		m_FileBrowser->Display();

		if (m_FileBrowser->HasSelected())
		{
			//TODO: Add FileBrowser Flags
			m_scriptFilePath = m_FileBrowser->GetSelected().u8string();
			m_SelectedActor->AddComponent<JinxScriptComponent>(m_Actor, m_Scene.GetJinxRuntime(), m_scriptFilePath);
			m_FileBrowser->ClearSelected();
			m_SelectedActor = nullptr;
		}

	}
}