#include "stdafx.h"
#include "EditorLayer.h"
#include "Core/Renderer/Renderer2D.h"
#include "Core/Scripting/Jinx.h"

#include <imgui/imgui.h>
#include "icon_font_cpp_headers/IconsFontAwesome5.h"

namespace SmolEngine
{
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
			rb = &GetComponent<Rigidbody2DComponent>();

			for (auto obj : GetActorList())
			{
				CONSOLE_INFO(std::string("Actor found : ") +  obj->GetName());
			}

			for (auto obj : GetActorListByTag("Default"))
			{
				NATIVE_INFO("Actor ID find by tag: {}", obj->GetID());
			}

			auto ground = GetChildByName("Ground");
			if (ground)
			{
				CONSOLE_INFO(std::string("Child found : ") + ground->GetName());
			}

		}

		void OnUpdate(DeltaTime deltaTime) override
		{
			if (Input::IsKeyPressed(KeyCode::Q))
			{
				rb->AddForce({ 20.0f, 1.0f });
			}
		}

		void OnDestroy() override {}

	private:
		Rigidbody2DComponent* rb = nullptr;
	};

	void EditorLayer::OnAttach()
	{
		m_FileBrowser = std::make_shared<ImGui::FileBrowser>();
		m_Scene = Scene::GetScene();
		s_EditorConsole = std::make_shared<EditorConsole>();

		s_EditorConsole->AddMessage(std::string("Console Successfully Initialized!"), LogLevel::Info);

		m_Texture = Texture2D::Create("Assets/Textures/Background.png");
		m_SheetTexture = Texture2D::Create("Assets/Textures/RPGpack_sheet_2X.png");

		m_HouseSubTexture = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 4.0f, 6.0f }, { 128.0f, 128.0f });
		m_FieldSubTexture = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 1.0f, 11.0f }, { 128.0f, 128.0f });

		m_Actor = m_Scene->CreateActor("Actor_1");
		m_Actor->AddComponent<Texture2DComponent>(m_Texture);
		m_Actor->AddComponent<ScriptComponent>().SetScript<CharMovementScript>(m_Actor);
		m_Actor->AddComponent<Rigidbody2DComponent>(m_Actor, Scene::GetScene()->GetWorld(), BodyType::Dynamic);

		m_CameraActor = m_Scene->CreateActor("Camera");
		m_CameraActor->AddComponent<CameraComponent>();

		auto& ground = m_Scene->CreateActor("Ground","TestTag");
		ground->GetComponent<TransfromComponent>().SetTranform(1.0f, -2.0f);;
		ground->AddComponent<Rigidbody2DComponent>(ground, Scene::GetScene()->GetWorld(), BodyType::Static);

		m_Scene->AddChild(m_Actor, ground);

		auto nameActor = m_Scene->FindActorByName("Ground");
		auto tagActor = m_Scene->FindActorByTag("TestTag");

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
		m_Scene->OnUpdate(deltaTime);
	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_Scene->OnEvent(event);
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

		//----------------------------------------STATES----------------------------------------//
		static bool createActorOpened;
		static bool showRenderer2Dstats;
		static bool showConsole = true;
		//----------------------------------------STATES----------------------------------------//

		if (ImGui::BeginMainMenuBar())
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


			if (ImGui::BeginMenu("Create"))
			{

				if (ImGui::MenuItem("Actor"))
				{
					createActorOpened = true;
				}

				if (ImGui::MenuItem("System // Not Implemented"))
				{

				}

				if (ImGui::MenuItem("Scene // Not Implemented"))
				{

				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Build & Play"))
			{
				if (ImGui::MenuItem("Play"))
				{
					m_Scene->OnPlay();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Renderer2D Stats"))
				{
					showRenderer2Dstats = true;
				}

				if (ImGui::MenuItem("Console"))
				{
					showConsole = true;
				}

				ImGui::EndMenu();
			}

		}
		ImGui::EndMainMenuBar();


		if (createActorOpened)
		{
			ImGui::Begin("Create Actor", &createActorOpened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);
			//ImGui::SetWindowPos(ImVec2{ (float)Application::GetApplication().GetWindowWidth() / 2 - 350, 
//(float)Application::GetApplication().GetWindowHeight() / 2 - 180 });
			ImGui::SetWindowSize("Create Actor", { 480, 200 });

			static char name[128] = "Default Actor";
			ImGui::NewLine();
			ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
			ImGui::PushItemWidth(200);
			ImGui::Separator();
			ImGui::NewLine();

			static char tag[128] = "Default";
			ImGui::InputText("Tag", tag, IM_ARRAYSIZE(tag));
			ImGui::PushItemWidth(200);
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::Button("Add", ImVec2{ 100, 25 }))
			{
				auto ref = m_Scene->CreateActor(name, tag);
				if (!ref)
				{
					s_EditorConsole->AddMessage(std::string("Actor was not created, it already exists"), LogLevel::Error);
				}

				createActorOpened = false;
			}
			ImGui::End();
		}

		if (showRenderer2Dstats)
		{
			if (ImGui::Begin("Renderer2D: Stats", &showRenderer2Dstats))
			{
				ImGui::Text("Active GPU: %s", reinterpret_cast<char const*>(renderer));
				ImGui::Text("Draw Calls: %d", Renderer2D::GetData().DrawCalls);
				ImGui::Text("Quads Rendered: %d", Renderer2D::GetData().QuadCount);
				ImGui::Text("Total Vertices: %d", Renderer2D::GetData().GetTotalVertexCount());
				ImGui::End();
			}
		}

		if (showConsole)
		{
			s_EditorConsole->Update(showConsole);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f});
		if(ImGui::Begin("Scene View"))
		{
			auto& m_FrameBuffer = m_Scene->GetFrameBuffer();
			ImVec2 ViewPortSize = ImGui::GetContentRegionAvail();

			if (ViewPortSize.x != m_ViewPortSize.x || ViewPortSize.y != m_ViewPortSize.y)
			{
				m_ViewPortSize = { ViewPortSize.x, ViewPortSize.y };
				m_FrameBuffer->OnResize(m_ViewPortSize.x, m_ViewPortSize.y);
				m_Scene->OnEditorResize(m_ViewPortSize.x, m_ViewPortSize.y);
			}
			uint32_t textureID = m_FrameBuffer->GetColorAttachmentID();
			ImGui::Image((void*)textureID, ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::End();
		}
		ImGui::PopStyleVar();

		if(ImGui::Begin("Hierarchy"));
		{
			for (auto actor: m_Scene->GetActorPool())
			{
				if (actor->IsDisabled) { continue; }

				if (ImGui::TreeNodeEx(actor->GetName().c_str()))
				{
					ImGui::TreePop();
				}

				if (ImGui::IsItemClicked())
				{
					m_SelectedActor = nullptr;
					m_SelectedActor = actor;
				}

			}

			ImGui::End();
		}

		ImGui::End();

		if(ImGui::Begin("Actor View"))
		{
			if (m_SelectedActor == nullptr)
			{
				ImGui::Text("No Actor selected");
			}
			else
			{
				ImGui::PushID(m_SelectedActor->GetName().c_str());
				if (ImGui::TreeNodeEx("Components", ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::TreeNode("Transform"))
					{
						auto& tranfrom = m_SelectedActor->GetComponent<TransfromComponent>();

						ImGui::InputFloat3("Transform", glm::value_ptr(tranfrom.WorldPos));
						ImGui::InputFloat("Rotation", &tranfrom.Rotation);
						ImGui::InputFloat2("Scale", glm::value_ptr(tranfrom.Scale));

						ImGui::TreePop();
					}


					if (m_SelectedActor->HasComponent<Rigidbody2DComponent>())
					{
						if (ImGui::TreeNode("Rigidbody2D"))
						{
							auto& rb = m_SelectedActor->GetComponent<Rigidbody2DComponent>();

							ImGui::NewLine();
							ImGui::Combo("Type", &rb.Rigidbody->m_Type, "Static\0Kinematic\0Dynamic\0\0");

							if (rb.Rigidbody->m_Type == 2)
							{
								ImGui::Separator();
								ImGui::InputFloat("Friction", &rb.Rigidbody->m_Friction);
								ImGui::Separator();
								ImGui::InputFloat("Density", &rb.Rigidbody->m_Density);
								ImGui::Separator();
								ImGui::InputFloat("Restitution", &rb.Rigidbody->m_Restitution);
								ImGui::Separator();
								ImGui::InputFloat("Gravity Scale", &rb.Rigidbody->m_GravityScale);
								ImGui::Separator();
								ImGui::Checkbox("Bullet Mode", &rb.Rigidbody->m_IsBullet);
							}

							ImGui::Separator();
							ImGui::Checkbox("Awake", &rb.Rigidbody->m_IsAwake);
							ImGui::Separator();
							ImGui::Checkbox("Allow Sleep", &rb.Rigidbody->m_canSleep);

							ImGui::TreePop();
						}
					}

					if (m_SelectedActor->HasComponent<Texture2DComponent>())
					{
						if (ImGui::TreeNode("Texture2D"))
						{
							if (m_SelectedActor->GetComponent<Texture2DComponent>().Texture != nullptr)
							{
								ImGui::Separator();
								ImGui::NewLine();
								ImGui::Image((void*)m_SelectedActor->GetComponent<Texture2DComponent>().Texture->GetID(), ImVec2{ 100, 100 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
								ImGui::SameLine();

								if (ImGui::Button("Change"))
								{
									m_FileBrowserState = FileBrowserFlags::Texture2dPath;
									m_FileBrowser->SetTitle("Select a texture");
									m_FileBrowser->SetTypeFilters({ ".png" });
									m_FileBrowser->Open();
								}

								ImGui::SameLine();
								ImGui::Checkbox("Texture2D Enabled", &m_SelectedActor->GetComponent<Texture2DComponent>().Enabled);
							}
							else
							{
								ImGui::NewLine();
								ImGui::Separator();

								if (ImGui::Button("New Texture2D"))
								{
									m_FileBrowserState = FileBrowserFlags::Texture2dPath;
									m_FileBrowser->SetTitle("Select a texture");
									m_FileBrowser->SetTypeFilters({ ".png" });
									m_FileBrowser->Open();
								}
							}

							ImGui::NewLine();
							ImGui::Separator();
							ImGui::ColorEdit3("Color", glm::value_ptr(m_SelectedActor->GetComponent<Texture2DComponent>().Color));
							ImGui::TreePop();
						}
					}

					if (m_SelectedActor->HasComponent<JinxScriptComponent>())
					{
						if (ImGui::TreeNode("Jinx Script"))
						{
							ImGui::Checkbox("JinxScript Enabled", &m_SelectedActor->GetComponent<JinxScriptComponent>().Enabled);
							ImGui::TreePop();
						}
					}

					if (m_SelectedActor->HasComponent<ScriptComponent>())
					{
						if (ImGui::TreeNode("C++ Script"))
						{
							ImGui::Checkbox("C++ Script Enabled", &m_SelectedActor->GetComponent<ScriptComponent>().Enabled);
							ImGui::TreePop();
						}
					}

					if (m_SelectedActor->HasComponent<CameraComponent>())
					{
						if (ImGui::TreeNode("Camera"))
						{
							static float zoomLevel;
							ImGui::InputFloat("Zoom Level", &zoomLevel);
							ImGui::Separator();
							ImGui::Checkbox("Camera Enabled", &m_SelectedActor->GetComponent<CameraComponent>().Enabled);
							ImGui::SameLine();
							if (ImGui::Button("Update"))
							{
								m_SelectedActor->GetComponent<CameraComponent>().Camera->SetZoom(zoomLevel);
							}

							ImGui::TreePop();
						}
					}

					ImGui::TreePop();
					ImGui::PopID();
				}

				ImGui::PushID(m_SelectedActor->GetName().c_str());
				if (ImGui::TreeNodeEx("Actions", ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::TreeNode("Add/Delete Component"))
					{
						static int current_item = 0;
						ImGui::Combo("Component", &current_item, "None\0Texture2D\0JinxScript\0Rigidbody2D\0AnimationContoller\0ParticleSystem\0\0");
						ImGui::Separator();

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
								if (!m_SelectedActor->HasComponent<Texture2DComponent>())
								{
									m_SelectedActor->AddComponent<Texture2DComponent>();
									break;
								}

								CONSOLE_WARN("Actor already has Tetxure2D component");
								EDITOR_WARN("Actor <{}> already has Tetxure2D component.", m_SelectedActor->GetName());
								break;
							}
							case (uint32_t)ComponentItem::JinxScript:
							{
								if (!m_SelectedActor->HasComponent<JinxScriptComponent>())
								{
									m_FileBrowserState = FileBrowserFlags::ScriptPath;
									m_FileBrowser->SetTitle("Select a Jinx script");
									m_FileBrowser->SetTypeFilters({ ".jinx" });
									m_FileBrowser->Open();
									break;
								}

								CONSOLE_WARN("Actor already has Script component");
								EDITOR_WARN("Actor <{}> already has Script component.", m_SelectedActor->GetName());
								break;
							}
							case (uint32_t)ComponentItem::Rigidbody2D:
							{
								if (!m_SelectedActor->HasComponent<Rigidbody2DComponent>())
								{
									m_SelectedActor->AddComponent<Rigidbody2DComponent>(m_SelectedActor, Scene::GetScene()->GetWorld(), BodyType::Static);

									break;
								}

								CONSOLE_WARN("Actor already has Rigidbody2D component");
								EDITOR_WARN("Actor <{}> already has Rigidbody2D component.", m_SelectedActor->GetName());
							}
							default:
								break;
							}

							current_item = 0;
						}

						ImGui::SameLine();
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
								if (m_SelectedActor->HasComponent<Texture2DComponent>())
								{
									m_SelectedActor->DeleteComponent<Texture2DComponent>();
									break;
								}

								CONSOLE_WARN("Actor does not have Tetxure2D component");
								EDITOR_WARN("Actor <{}> does not have Tetxure2D component.", m_SelectedActor->GetName());
								break;
							}
							case (uint32_t)ComponentItem::JinxScript:
							{
								if (m_SelectedActor->HasComponent<JinxScriptComponent>())
								{
									m_SelectedActor->DeleteComponent<JinxScriptComponent>();
									break;
								}

								CONSOLE_WARN("Actor does not have Script component");
								EDITOR_WARN("Actor <{}> does not have Script component.", m_SelectedActor->GetName());
								break;
							}
							case (uint32_t)ComponentItem::Rigidbody2D:
							{
								if (m_SelectedActor->HasComponent<Rigidbody2DComponent>())
								{
									m_SelectedActor->DeleteComponent<Rigidbody2DComponent>();
									break;
								}

								CONSOLE_WARN("Actor does not have Rigidbody2D component");
								EDITOR_WARN("Actor <{}> does not have Rigidbody2D component.", m_SelectedActor->GetName());
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
							m_SelectedActor->GetName() = std::string(name);
						}

						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Default"))
					{
						if (ImGui::Button("Save", ImVec2{ 60, 25 }))
						{
							//!TODO: Serialize Actor Class
						}

						ImGui::SameLine();

						if (ImGui::Button("Delete", ImVec2{ 60, 25 }))
						{
							//TODO: Fix DeleteActorImmediately method
							m_Scene->DeleteActor(m_SelectedActor);
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
					ImGui::PopID();
				}
			}

			ImGui::End();
		}

		m_FileBrowser->Display();

		if (m_FileBrowser->HasSelected())
		{
			m_FilePath = m_FileBrowser->GetSelected().u8string();

			switch (m_FileBrowserState)
			{
			case FileBrowserFlags::ScriptPath:

				m_SelectedActor->AddComponent<JinxScriptComponent>(m_Actor, m_Scene->GetJinxRuntime(), m_FilePath);
				ResetFileBrowser();
				break;
			case FileBrowserFlags::Texture2dPath:
			{
				m_SelectedActor->GetComponent<Texture2DComponent>().Texture = Texture2D::Create(m_FilePath);
				ResetFileBrowser();
				break;
			}
			default:
				break;
			}
		}

	}

	void EditorLayer::ResetFileBrowser()
	{
		m_FileBrowser->ClearSelected();
		m_SelectedActor = nullptr;
		m_FilePath = "";
		m_FileBrowserState = FileBrowserFlags::None;
	}
}