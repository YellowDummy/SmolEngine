#include "stdafx.h"
#include "../../GameX/Scripts.h"

#include "EditorLayer.h"
#include "icon_font_cpp_headers/IconsFontAwesome5.h"
#include "Core/Renderer/Renderer2D.h"
#include "Core/Scripting/Jinx.h"
#include "Core/ECS/Scene.h"

#include <imgui/imgui.h>
#include <fstream>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

namespace SmolEngine
{
	void EditorLayer::OnAttach()
	{
		m_FileBrowser = std::make_unique<ImGui::FileBrowser>();
		m_SettingsWindow = std::make_unique<SettingsWindow>();
		m_ActorCreationWindow = std::make_unique<ActorCreationWindow>();

		m_EditorConsole = EditorConsole::GetConsole();

		m_Scene = Scene::GetScene();

		m_Scene->GetSceneData().m_filePath = "C:/Dev/SmolEngine/SmolEngine-Editor/TestScene.scene";
		m_Scene->GetSceneData().m_ID = 12233456445;

		m_Texture = Texture2D::Create("Assets/Textures/Background.png");
		m_SheetTexture = Texture2D::Create("Assets/Textures/RPGpack_sheet_2X.png");

		m_HouseSubTexture = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 4.0f, 6.0f }, { 128.0f, 128.0f });
		m_FieldSubTexture = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 1.0f, 11.0f }, { 128.0f, 128.0f });

		m_Actor = m_Scene->CreateActor("Actor_1", "Player");
		m_Actor->AddComponent<Texture2DComponent>("Assets/Textures/Background.png");

		m_Scene->SetScriptToActor<CharMovementScript>(m_Actor);

		m_Actor->AddComponent<Rigidbody2DComponent>(m_Actor, Scene::GetScene()->GetWorld(), BodyType::Dynamic);

		m_CameraActor = m_Scene->CreateActor("Camera");
		m_CameraActor->AddComponent<CameraComponent>();
		m_Scene->SetScriptToActor<CameraMovementScript>(m_CameraActor);

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

		m_Scene->Save(std::string("C:/Dev/SmolEngine/SmolEngine-Editor/TestScene.scene"));

	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(DeltaTime deltaTime)
	{
		if (isSceneViewFocused)
		{
			m_Scene->m_EditorCamera->OnUpdate(deltaTime);
		}

		m_Scene->OnUpdate(deltaTime);
	}

	void EditorLayer::OnEvent(Event& event)
	{
		if (isSceneViewFocused)
		{
			m_Scene->m_EditorCamera->OnEvent(event);
		}

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

		//---------------------------------------WINDOW-STATES----------------------------------------//
		static bool showActorCreationWindow;
		static bool showRenderer2Dstats;
		static bool showConsole = true;
		static bool showGameView = false;
		static bool showSettingsWindow = false;
		//---------------------------------------WINDOW-STATES----------------------------------------//

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 25.0f, 10.0f });
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

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scene"))
			{

				if (ImGui::MenuItem("Create New"))
				{
					m_FileBrowser = std::make_unique<ImGui::FileBrowser>(ImGuiFileBrowserFlags_EnterNewFilename);

					m_FileBrowser->SetTypeFilters({ ".scene" });
					m_FileBrowser->SetTitle("New Scene");
					m_FileBrowserState = FileBrowserFlags::SceneCreate;
					m_FileBrowser->Open();
				}

				if (ImGui::MenuItem("Save Current"))
				{
					m_Scene->Save(m_Scene->GetSceneData().m_filePath);
				}

				if (ImGui::MenuItem("Save as"))
				{
					m_FileBrowser = std::make_unique<ImGui::FileBrowser>(ImGuiFileBrowserFlags_EnterNewFilename);

					m_FileBrowser->SetTypeFilters({ ".scene" });
					m_FileBrowser->SetTitle("Save as");
					m_FileBrowserState = FileBrowserFlags::SceneSave;
					m_FileBrowser->Open();
				}

				if (ImGui::MenuItem("Load Scene"))
				{
					m_FileBrowser->SetTypeFilters({ ".scene" });
					m_FileBrowser->SetTitle("Load Scene");
					m_FileBrowserState = FileBrowserFlags::SceneLoad;
					m_FileBrowser->Open();
				}

				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("Actor"))
			{

				if (ImGui::MenuItem("New Actor"))
				{
					showActorCreationWindow = true;
				}

				if (ImGui::MenuItem("New System"))
				{

				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Simulation"))
			{
				if (ImGui::MenuItem("Play"))
				{
					m_Scene->OnPlay();
				}

				if (ImGui::MenuItem("Stop"))
				{
					m_Scene->OnEndPlay();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Build"))
			{

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Settings"))
				{
					showSettingsWindow = true;
				}

				if (ImGui::MenuItem("Renderer2D Stats"))
				{
					showRenderer2Dstats = true;
				}

				if (ImGui::MenuItem("Console"))
				{
					showConsole = true;
				}

				if (ImGui::MenuItem("Game View"))
				{
					showGameView = true;
				}

				ImGui::EndMenu();
			}

		}
		ImGui::EndMainMenuBar();
		ImGui::PopStyleVar();

		m_SettingsWindow->Update(showSettingsWindow, m_Scene);

		m_ActorCreationWindow->Update(showActorCreationWindow, m_Scene);

		m_EditorConsole->Update(showConsole);

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

		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
			ImGui::Begin("Scene View");
			{
				if (ImGui::IsWindowFocused()) { isSceneViewFocused = true; }
				else { isSceneViewFocused = false; }

				auto& m_FrameBuffer = m_Scene->m_EditorCamera->m_FrameBuffer;
				ImVec2 ViewPortSize = ImGui::GetContentRegionAvail();

				if (ViewPortSize.x != m_ViewPortSize.x || ViewPortSize.y != m_ViewPortSize.y)
				{
					m_ViewPortSize = { ViewPortSize.x, ViewPortSize.y };
					m_Scene->OnSceneViewResize(m_ViewPortSize.x, m_ViewPortSize.y);
				}

				size_t textureID = m_FrameBuffer->GetColorAttachmentID();
				ImGui::Image((void*)textureID, ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			}
			ImGui::End();
			ImGui::PopStyleVar();
		}

		{
			if (showGameView)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
				ImGui::Begin("Game View", &showGameView);
				{

					if (ImGui::IsWindowFocused()) { isGameViewFocused = true; }
					else { isGameViewFocused = false; }

					ImVec2 ViewPortSize = ImGui::GetContentRegionAvail();

					if (ViewPortSize.x != m_GameViewPortSize.x || ViewPortSize.y != m_GameViewPortSize.y)
					{
						m_GameViewPortSize = { ViewPortSize.x, ViewPortSize.y };
						m_Scene->OnGameViewResize(m_GameViewPortSize.x, m_GameViewPortSize.y);
					}

					auto camGroup = m_Scene->GetSceneData().m_Registry.view<CameraComponent>();

					for (auto obj : camGroup)
					{
						auto& cameraComponent = camGroup.get<CameraComponent>(obj);
						if (cameraComponent.isSelected)
						{
							size_t ImageTextureID = cameraComponent.Camera->m_FrameBuffer->GetColorAttachmentID();
							ImGui::Image((void*)ImageTextureID, ImVec2{ m_GameViewPortSize.x, m_GameViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
						}
					}


				}
				ImGui::End();
				ImGui::PopStyleVar();
			}

		}
	

		ImGui::Begin("Hierarchy");
		{
			static char name[128];
			ImGui::InputTextWithHint("Search", "Name", name, IM_ARRAYSIZE(name));
			ImGui::Separator();

			std::string sceneStr;

			if (m_Scene->GetSceneData().m_Name == std::string(""))
			{
				sceneStr = "Scene (" + std::to_string(m_Scene->GetSceneData().m_ID) + ")";
			}
			else
			{
				sceneStr = "Scene (" + m_Scene->GetSceneData().m_Name + ")";
			}

			if (ImGui::TreeNodeEx(sceneStr.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (auto actor : m_Scene->GetActorPool())
				{
					if (actor->IsDisabled) { continue; }

					auto result = actor->GetName().find(name);
					if (result == std::string::npos)
					{
						continue;
					}

					if (ImGui::TreeNodeEx(actor->GetName().c_str()))
					{
						if (ImGui::TreeNodeEx("Childs"))
						{
							if (actor->GetChilds().empty())
							{
								ImGui::BulletText("Empty");
							}
							else
							{
								for (auto obj : actor->GetChilds())
								{
									ImGui::BulletText(obj->GetName().c_str());
								}
							}

							ImGui::TreePop();
						}

						ImGui::TreePop();
					}

					if (ImGui::IsItemClicked())
					{
						m_SelectedActor = nullptr;
						m_SelectedActor = actor;
					}

				}

				ImGui::TreePop();
			}

		}
		ImGui::End();

		ImGui::End();

		ImGui::Begin("Inspector");
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
						auto& comp = m_SelectedActor->GetComponent<Texture2DComponent>();

						if (ImGui::TreeNode("Texture2D"))
						{
							if (comp.Texture != nullptr)
							{
								ImGui::Separator();
								ImGui::NewLine();
								ImGui::Image((void*)comp.Texture->GetID(), ImVec2{ 100, 100 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
								ImGui::SameLine();

								if (ImGui::Button("Change"))
								{
									m_FileBrowserState = FileBrowserFlags::Texture2dPath;
									m_FileBrowser->SetTitle("Select a texture");
									m_FileBrowser->SetTypeFilters({ ".png" });
									m_FileBrowser->Open();
								}

								ImGui::SameLine();
								ImGui::Checkbox("Texture2D Enabled", &comp.Enabled);
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
							ImGui::ColorEdit3("Color", glm::value_ptr(comp.Color));
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
						auto& comp = m_SelectedActor->GetComponent<CameraComponent>();
						auto& transform = m_SelectedActor->GetComponent<TransfromComponent>();

						if (ImGui::TreeNode("Camera Controller"))
						{
							ImGui::InputFloat("Zoom Level", &comp.Camera->m_ZoomLevel);
							ImGui::Separator();
							ImGui::Checkbox("Main Camera", &comp.isSelected);
							ImGui::Separator();
							ImGui::Checkbox("Camera Enabled", &comp.Enabled);
							ImGui::TreePop();
						}
					}

					ImGui::TreePop();
					ImGui::PopID();
				}

				ImGui::Separator();
				ImGui::PushID(m_SelectedActor->GetName().c_str());
				if (ImGui::TreeNodeEx("Actions", ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::TreeNode("Add/Delete Component"))
					{
						static int current_item = 0;
						ImGui::Combo("Component", &current_item, "None\0Texture2D\0JinxScript\0Rigidbody2D\0CameraController\0AnimationContoller\0ParticleSystem\0\0");
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
									m_FileBrowserState = FileBrowserFlags::Texture2dPath;
									m_FileBrowser->SetTitle("Select a texture");
									m_FileBrowser->SetTypeFilters({ ".png" });
									m_FileBrowser->Open();
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
								break;
							}
							case (uint32_t)ComponentItem::CameraController:
							{
								if (!m_SelectedActor->HasComponent<CameraComponent>())
								{
									m_SelectedActor->AddComponent<CameraComponent>();
									break;
								}

								CONSOLE_WARN("Actor already has CameraController component");
								EDITOR_WARN("Actor <{}> already has CameraController component.", m_SelectedActor->GetName());
								break;
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
		}
		ImGui::End();

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
				m_SelectedActor->GetComponent<Texture2DComponent>().TexturePath = m_FilePath;
				m_SelectedActor->GetComponent<Texture2DComponent>().Texture = Texture2D::Create(m_FilePath);
				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::SceneCreate:
			{
				m_FileBrowser = std::make_unique<ImGui::FileBrowser>();
				m_SelectedActor = nullptr;

				m_Scene->CreateScene(m_FilePath);
				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::SceneSave:
			{
				m_FileBrowser = std::make_unique<ImGui::FileBrowser>();
				m_Scene->Save(m_FilePath);
				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::SceneLoad:
			{
				m_SelectedActor = nullptr;

				m_Scene->Load(m_FilePath);
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
		m_FilePath = "";
		m_FileBrowserState = FileBrowserFlags::None;
	}
}