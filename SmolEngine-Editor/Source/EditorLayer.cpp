#include "stdafx.h"
#include "../../GameX/CppScriptingExamples.h"

#include "EditorLayer.h"
#include "icon_font_cpp_headers/IconsFontAwesome5.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "Core/Renderer/Renderer2D.h"
#include "Core/Animation/Animation2D.h"
#include "Core/Scripting/Jinx.h"
#include "Core/ECS/Scene.h"
#include "Core/Audio/AudioClip.h"
#include "Core/Audio/AudioSource.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <fstream>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include "Core/ImGui/NodeEditor/imnodes.h"

namespace SmolEngine
{

	void EditorLayer::OnAttach()
	{
		m_FileBrowser = std::make_shared<ImGui::FileBrowser>();

		m_AnimationPanel = std::make_unique<AnimationPanel>();
		m_SettingsWindow = std::make_unique<SettingsWindow>();
		m_ActorCreationWindow = std::make_unique<ActorCreationWindow>();

		m_EditorConsole = EditorConsole::GetConsole();

		m_Scene = Scene::GetScene();
		m_Scene->CreateScene(std::string("C:/Dev/SmolEngine/SmolEngine-Editor/TestScene.smolscene"), std::string("TestScene.smolscene"));

		//Temp
		m_Scene->RegistryScript<CharMovementScript>(std::string("CharMovementScript"));
		m_Scene->RegistryScript<CameraMovementScript>(std::string("CameraMovementScript"));

		m_Texture = Texture2D::Create("Assets/Textures/Background.png");
		m_SheetTexture = Texture2D::Create("Assets/Textures/RPGpack_sheet_2X.png");

		auto test2 = Texture2D::Create("Assets/Textures/7yWvW.png");


		m_HouseSubTexture = SubTexture2D::GenerateFromCoods(m_SheetTexture, { 4.0f, 6.0f }, { 128.0f, 128.0f });
		m_Scene->m_TestSub = SubTexture2D::GenerateFromCoods(test2, { 2.0f, 1.0f }, { 260, 513 }, { 1, 1 });

		m_Actor = m_Scene->CreateActor("Actor_1", "Player");
		m_Actor->AddComponent<Texture2DComponent>("Assets/Textures/Background.png");
		m_Actor->AddComponent<Rigidbody2DComponent>(m_Actor, BodyType::Dynamic);
		m_Scene->AttachScript(std::string("CharMovementScript"), m_Actor);

		m_CameraActor = m_Scene->CreateActor("Camera");
		m_CameraActor->AddComponent<CameraComponent>();
		m_Scene->AttachScript(std::string("CameraMovementScript"), m_CameraActor);

		auto& ground = m_Scene->CreateActor("Ground","TestTag");
		ground->GetComponent<TransformComponent>().SetTranform(1.0f, -2.0f);;
		ground->AddComponent<Rigidbody2DComponent>(ground, BodyType::Static);

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
		if (isSceneViewFocused)
		{
			m_Scene->m_EditorCamera->OnUpdate(deltaTime);
		}


		m_Scene->OnUpdate(deltaTime);
	}

	void EditorLayer::OnEvent(Event& event)
	{
		if (event.m_EventType == (uint32_t)EventType::S_WINDOW_RESIZE)
		{
			auto& e = static_cast<WindowResizeEvent&>(event);

			m_ViewPortSize = { e.GetWidth() , e.GetHeight() };
		}

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

		static bool showAnimationPanel = false;

		//TEMP
		static bool showNodeEditorTest = false;

		//---------------------------------------WINDOW-STATES----------------------------------------//

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 10.0f });
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
				if (!m_Scene->m_InPlayMode)
				{
					if (ImGui::MenuItem("New"))
					{
						m_FileBrowser = std::make_unique<ImGui::FileBrowser>(ImGuiFileBrowserFlags_EnterNewFilename);

						m_FileBrowser->SetTypeFilters({ ".smolscene" });
						m_FileBrowser->SetTitle("New Scene");
						m_FileBrowserState = FileBrowserFlags::SceneCreate;
						m_FileBrowser->Open();
					}

					if (ImGui::MenuItem("Save"))
					{
						m_Scene->SaveCurrentScene();
					}

					if (ImGui::MenuItem("Save as"))
					{
						m_FileBrowser = std::make_unique<ImGui::FileBrowser>(ImGuiFileBrowserFlags_EnterNewFilename);

						m_FileBrowser->SetTypeFilters({ ".smolscene" });
						m_FileBrowser->SetTitle("Save as");
						m_FileBrowserState = FileBrowserFlags::SceneSave;
						m_FileBrowser->Open();
					}

					if (ImGui::MenuItem("Load"))
					{
						m_SelectedActor = nullptr;

						m_FileBrowser->SetTypeFilters({ ".smolscene" });
						m_FileBrowser->SetTitle("Load Scene");
						m_FileBrowserState = FileBrowserFlags::SceneLoad;
						m_FileBrowser->Open();
					}
				}

				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("Actors & Systems"))
			{
				if (!m_Scene->m_InPlayMode)
				{
					if (ImGui::MenuItem("New Actor"))
					{
						showActorCreationWindow = true;
					}

					if (ImGui::MenuItem("New System"))
					{

					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Animation"))
			{
				if (ImGui::MenuItem("New Clip"))
				{
					showAnimationPanel = true;
					m_AnimationPanel->m_AnimationClip = std::make_unique<Animation2D>();
				}

				if (ImGui::MenuItem("Load Clip"))
				{
					m_FileBrowserState = FileBrowserFlags::LoadAnimationClip;

					m_FileBrowser->SetTitle("Load Clip");
					m_FileBrowser->SetTypeFilters({ ".smolanim" });
					m_FileBrowser->Open();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Simulation"))
			{
				if (ImGui::MenuItem("Play Mode"))
				{
					if (!m_Scene->m_InPlayMode)
					{
						m_Scene->OnPlay();
					}
					else
					{
						CONSOLE_WARN("The scene is already in play mode!");
					}
				}

				if (ImGui::MenuItem("Stop"))
				{
					if (m_Scene->m_InPlayMode)
					{
						size_t selectedActorID = 0;
						if (m_SelectedActor != nullptr)
						{
							selectedActorID = m_SelectedActor->GetID();
						}

						m_SelectedActor = nullptr;
						m_Scene->OnEndPlay();
						m_SelectedActor = m_Scene->FindActorByID(selectedActorID);
					}
					else
					{
						CONSOLE_WARN("The scene is not in play mode!");
					}
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

		m_AnimationPanel->Update(showAnimationPanel);

		m_EditorConsole->Update(showConsole);

		if (showRenderer2Dstats)
		{
			if (ImGui::Begin("Renderer2D: Stats", &showRenderer2Dstats))
			{

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
				ImGui::Image((void*)(intptr_t)textureID, ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			}
			ImGui::End();
			ImGui::PopStyleVar();
		}

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
						ImGui::Image((void*)(intptr_t)ImageTextureID, ImVec2{ m_GameViewPortSize.x, m_GameViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
					}
				}


			}
			ImGui::End();
			ImGui::PopStyleVar();

		}

		//TEMP
		if (showNodeEditorTest)
		{
			if (ImGui::Begin("Node Editor Test"), &showNodeEditorTest)
			{
				const int hardcoded_node_id = 1;

				imnodes::BeginNodeEditor();

				imnodes::BeginNode(hardcoded_node_id);
				{
					imnodes::BeginNodeTitleBar();
					ImGui::TextUnformatted("output node");
					imnodes::EndNodeTitleBar();

					const int output_attr_id = 2;
					imnodes::BeginOutputAttribute(output_attr_id);
					// in between Begin|EndAttribute calls, you can call ImGui
					// UI functions
					ImGui::Text("output pin");
					imnodes::EndOutputAttribute();
				}
				imnodes::EndNode();

				imnodes::EndNodeEditor();

			}
			ImGui::End();
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

			ImGui::BeginChild("Scene");
			{
				if (ImGui::TreeNodeEx(sceneStr.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					for (auto actor : m_Scene->GetSortedActorList())
					{
						if (actor->IsDisabled) { continue; }

						auto result = actor->GetName().find(name);
						if (result == std::string::npos)
						{
							continue;
						}

						if (ImGui::TreeNodeEx(actor->GetName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow))
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

						if (ImGui::IsItemClicked(1))
						{
							m_SelectionFlags = SelectionFlags::Actions;

							m_SelectedActor = nullptr;
							m_SelectedActor = actor;
						}

						if (ImGui::IsItemClicked())
						{
							m_SelectionFlags = SelectionFlags::Inspector;
							m_SelectedActor = nullptr;
							m_SelectedActor = actor;
						}

					}

					ImGui::TreePop();
				}


				if (m_SelectionFlags == SelectionFlags::Actions)
				{
					ImGui::OpenPopup("ActionPopup");
					m_SelectionFlags = SelectionFlags::None;
				}

				if (ImGui::BeginPopup("ActionPopup"))
				{
					ImGui::MenuItem(m_SelectedActor->GetName().c_str(), NULL, false, false);
					ImGui::Separator();

					if (ImGui::MenuItem("Save")) {}
					if (ImGui::MenuItem("Save as", "Ctrl+O")) {}

					if (ImGui::BeginMenu("Rename"))
					{
						ImGui::InputTextWithHint("Name", m_SelectedActor->GetName().c_str(), &m_SelectedActor->GetName());
						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Change Tag"))
					{
						ImGui::InputTextWithHint("Tag", m_SelectedActor->GetTag().c_str(), &m_SelectedActor->GetTag());


						ImGui::EndMenu();
					}

					if (ImGui::MenuItem("Dublicate", "Ctrl+C"))
					{
						m_Scene->DuplicateActor(m_SelectedActor);
					}

					if (ImGui::MenuItem("Delete"))
					{
						m_Scene->DeleteActor(m_SelectedActor);
					}

					ImGui::EndPopup();
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();

		ImGui::Begin("Inspector");
		{
			ImGui::BeginChild("InspectorChild");

			if (m_SelectedActor == nullptr || m_SelectionFlags != SelectionFlags::Inspector)
			{
				ImGui::Text("No Actor selected");
			}
			else
			{
				ImGui::Text(m_SelectedActor->GetName().c_str());
				ImGui::Separator();

				ImGui::PushID(m_SelectedActor->GetName().c_str());
				if (ImGui::TreeNodeEx("Components", ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::TreeNode("Transform"))
					{
						auto& tranfrom = m_SelectedActor->GetComponent<TransformComponent>();

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
							ImGui::Combo("Body Type", &rb.Rigidbody->m_Type, "Static\0Kinematic\0Dynamic\0\0");
							ImGui::Separator();
							ImGui::Combo("Shape Type", &rb.Rigidbody->m_ShapeType, "Box\0Circle\0\0");
							ImGui::Separator();

							if (rb.Rigidbody->m_ShapeType == (int)ShapeType::Box)
							{
								ImGui::InputFloat2("Shape", glm::value_ptr(rb.Rigidbody->m_Shape));
							}
							else if (rb.Rigidbody->m_ShapeType == (int)ShapeType::Cirlce)
							{
								ImGui::InputFloat("Radius", &rb.Rigidbody->m_Radius);
								ImGui::Separator();
								ImGui::InputFloat2("Offset", glm::value_ptr(rb.Rigidbody->m_Offset));
							}

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
							ImGui::Checkbox("Show Shape", &rb.ShowShape);
							ImGui::Separator();
							ImGui::Checkbox("Awake", &rb.Rigidbody->m_IsAwake);
							ImGui::Separator();
							ImGui::Checkbox("Allow Sleep", &rb.Rigidbody->m_canSleep);

							ImGui::TreePop();
						}

						if (ImGui::IsItemClicked(1))
						{
							ImGui::OpenPopup("RbPopup");

						}

						if (ImGui::BeginPopup("RbPopup"))
						{
							ImGui::MenuItem("Rigidbody2D", NULL, false, false);
							if (ImGui::MenuItem("Delete"))
							{
								m_SelectedActor->DeleteComponent<Rigidbody2DComponent>();
							}

							ImGui::EndPopup();
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
								ImGui::Image((void*)(intptr_t)comp.Texture->GetID(), ImVec2{ 100, 100 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
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

						if (ImGui::IsItemClicked(1))
						{
							ImGui::OpenPopup("TexturePopup");

						}

						if (ImGui::BeginPopup("TexturePopup"))
						{
							ImGui::MenuItem("Texture2D", NULL, false, false);
							if (ImGui::MenuItem("Delete"))
							{
								m_SelectedActor->DeleteComponent<Texture2DComponent>();
							}

							ImGui::EndPopup();
						}
					}


					if (m_SelectedActor->HasComponent<Light2DComponent>())
					{
						auto& light = m_SelectedActor->GetComponent<Light2DComponent>();

						if (ImGui::TreeNode("Light2D"))
						{
							ImGui::InputFloat2("Size", glm::value_ptr(light.Light->m_Shape));
							ImGui::Separator();
							ImGui::InputFloat("Intensity", &light.Light->intensity);
							if (light.Light->intensity > 1.0f) { light.Light->intensity = 1.0f; };
							ImGui::Separator();
							ImGui::ColorEdit3("Light Color", glm::value_ptr(light.Light->m_Color));
							ImGui::TreePop();
						}

						if (ImGui::IsItemClicked(1))
						{
							ImGui::OpenPopup("LightPopup");

						}

						if (ImGui::BeginPopup("LightPopup"))
						{
							ImGui::MenuItem("Light2D", NULL, false, false);
							if (ImGui::MenuItem("Delete"))
							{
								m_SelectedActor->DeleteComponent<Light2DComponent>();
							}

							ImGui::EndPopup();
						}
					}

					if (m_SelectedActor->HasComponent<JinxScriptComponent>())
					{
						if (ImGui::TreeNode("Jinx Script"))
						{
							ImGui::Checkbox("JinxScript Enabled", &m_SelectedActor->GetComponent<JinxScriptComponent>().Enabled);
							ImGui::TreePop();
						}

						if (ImGui::IsItemClicked(1))
						{
							ImGui::OpenPopup("JinxScriptPopup");

						}

						if (ImGui::BeginPopup("JinxScriptPopup"))
						{
							ImGui::MenuItem("Jinx Script", NULL, false, false);
							if (ImGui::MenuItem("Delete"))
							{
								m_SelectedActor->DeleteComponent<JinxScriptComponent>();
							}

							ImGui::EndPopup();
						}
					}

					if (m_SelectedActor->HasComponent<ScriptObject>())
					{
						if (ImGui::TreeNode("C++ Script"))
						{
							auto& ref = m_SelectedActor->GetComponent<ScriptObject>();

							std::string typeName = "Type: " + ref.keyName;
							ImGui::Text(typeName.c_str());

							for (auto val : m_SelectedActor->m_OutValues)
							{
								switch (val->Value.index())
								{

								case (uint32_t)OutValueType::Float:
								{
									ImGui::InputFloat(val->Key.c_str(), &std::get<float>(val->Value));
									break;
								}
								case (uint32_t)OutValueType::Int:
								{
									ImGui::InputInt(val->Key.c_str(), &std::get<int>(val->Value));
									break;
								}
								case (uint32_t)OutValueType::String:
								{
									ImGui::InputText(val->Key.c_str(), val->stringBuffer, IM_ARRAYSIZE(val->stringBuffer));
									if (std::get<std::string>(val->Value).c_str() != val->stringBuffer)
									{
										std::get<std::string>(val->Value) = val->stringBuffer;
									}

									break;
								}
								default:
									break;
								}
							}

							ImGui::Checkbox("C++ Script Enabled", &ref.Enabled);
							ImGui::TreePop();
						}

						if (ImGui::IsItemClicked(1))
						{
							ImGui::OpenPopup("CppScriptPopup");

						}

						if (ImGui::BeginPopup("CppScriptPopup"))
						{
							ImGui::MenuItem("C++ Script", NULL, false, false);
							if (ImGui::MenuItem("Delete"))
							{
								m_SelectedActor->DeleteComponent<ScriptObject>();
							}

							ImGui::EndPopup();
						}

					}

					if (m_SelectedActor->HasComponent<CameraComponent>())
					{
						auto& comp = m_SelectedActor->GetComponent<CameraComponent>();
						auto& transform = m_SelectedActor->GetComponent<TransformComponent>();

						if (ImGui::TreeNode("Camera Controller"))
						{
							ImGui::InputFloat("Zoom Level", &comp.Camera->m_ZoomLevel);
							ImGui::Separator();
							ImGui::Checkbox("Main Camera", &comp.isSelected);
							ImGui::Separator();
							ImGui::Checkbox("Camera Enabled", &comp.Enabled);
							ImGui::TreePop();
						}

						if (ImGui::IsItemClicked(1))
						{
							ImGui::OpenPopup("CameraPopup");

						}

						if (ImGui::BeginPopup("CameraPopup"))
						{
							ImGui::MenuItem("Camera Controller", NULL, false, false);
							if (ImGui::MenuItem("Delete"))
							{
								m_SelectedActor->DeleteComponent<CameraComponent>();
							}

							ImGui::EndPopup();
						}
					}

					if (m_SelectedActor->HasComponent<Animation2DControllerComponent>())
					{
						auto& controller = m_SelectedActor->GetComponent<Animation2DControllerComponent>();

						if (ImGui::TreeNode("Animation2D Controller"))
						{
							if (ImGui::TreeNodeEx("Animation Clips", ImGuiTreeNodeFlags_DefaultOpen))
							{
								if (controller.AnimationController->m_Clips.size() == 0)
								{
									ImGui::BulletText("Empty");
								}

								for (auto& pair : controller.AnimationController->m_Clips)
								{
									auto& [key, clip] = pair;

									std::stringstream ss;
									ss << "Clip #" << clip->Clip->m_ClipName;

									if (ImGui::TreeNodeEx(ss.str().c_str(), ImGuiTreeNodeFlags_Bullet))
									{
										static std::string name = clip->Clip->m_ClipName;

										ImGui::InputText("Name", &name);
										if (ImGui::SmallButton("Set Name"))
										{
											clip->Clip->m_ClipName = name;
										}

										ImGui::Checkbox("Default Clip", &clip->IsDefaultClip);

										ImGui::SameLine();
										if (ImGui::SmallButton("Play"))
										{
											controller.AnimationController->Reset();
											controller.AnimationController->PlayClip(clip->Clip->m_ClipName);
										}

										ImGui::SameLine();
										if (ImGui::SmallButton("Stop"))
										{
											controller.AnimationController->Reset();
										}

										ImGui::TreePop();
									}
								}

								ImGui::TreePop();
							}

							ImGui::NewLine();
							if (ImGui::Button("Load Clip"))
							{
								m_FileBrowserState = FileBrowserFlags::LoadClipController;
								m_FileBrowser->SetTitle("Select Clip");
								m_FileBrowser->SetTypeFilters({ ".smolanim" });
								m_FileBrowser->Open();
							}

							ImGui::TreePop();
						}

						if (ImGui::IsItemClicked(1))
						{
							ImGui::OpenPopup("AnimationPopUp");
						}

						if (ImGui::BeginPopup("AnimationPopUp"))
						{
							ImGui::MenuItem("Animation 2D Controller", NULL, false, false);
							if (ImGui::MenuItem("Delete"))
							{
								m_SelectedActor->DeleteComponent<Animation2DControllerComponent>();
							}

							ImGui::EndPopup();
						}

					}

					if (m_SelectedActor->HasComponent<AudioSourceComponent>())
					{
						auto& as = m_SelectedActor->GetComponent<AudioSourceComponent>();

						if (ImGui::TreeNode("Audio Source"))
						{

							if (ImGui::TreeNodeEx("Audio Clips", ImGuiTreeNodeFlags_DefaultOpen))
							{
								if (as.AS->m_AudioClips.size() == 0)
								{
									ImGui::BulletText("Empty");
								}
								else
								{
									for (auto pair : as.AS->m_AudioClips)
									{
										auto& [key, clip] = pair;

										std::stringstream ss;
										ss << "Clip #" << clip->ClipName;

										if (ImGui::TreeNodeEx(ss.str().c_str(), ImGuiTreeNodeFlags_Bullet))
										{
											static std::string name = clip->ClipName;
											ImGui::InputText("Name", &name);
											if (ImGui::SmallButton("Set Name"))
											{
												clip->ClipName = name;
											}

											ImGui::InputFloat("Volume", &clip->Volume);

											if (clip->HasWorldPosition)
											{
												ImGui::InputFloat3("Position", glm::value_ptr(clip->WorldPos));
											}

											if (ImGui::Button("Play"))
											{
												as.AS->DebugPlay(clip);
											}
											ImGui::SameLine();
											if (ImGui::Button("Stop"))
											{
												as.AS->DebugStop(clip);
											}

											ImGui::Checkbox("Looping", &clip->IsLooping);
											ImGui::Checkbox("Default Clip", &clip->isDefaultClip);
											ImGui::Checkbox("3D Space", &clip->HasWorldPosition);

											ImGui::TreePop();
										}
									}
								}

								ImGui::TreePop();
							}


							ImGui::NewLine();
							if (ImGui::Button("Load Clip"))
							{
								m_FileBrowserState = FileBrowserFlags::LoadAudioClip;
								m_FileBrowser->SetTitle("Select a sound");
								m_FileBrowser->SetTypeFilters({ ".wav" });
								m_FileBrowser->Open();
							}

							ImGui::SameLine();
							ImGui::Checkbox("Play On Awake", &as.AS->m_PlayOnAwake);

							ImGui::TreePop();
						}

						if (ImGui::IsItemClicked(1))
						{
							ImGui::OpenPopup("ASPopup");

						}

						if (ImGui::BeginPopup("ASPopup"))
						{
							ImGui::MenuItem("Audio Source", NULL, false, false);
							if (ImGui::MenuItem("Delete"))
							{
								m_SelectedActor->DeleteComponent<AudioSourceComponent>();
							}

							ImGui::EndPopup();
						}
					}


					ImGui::TreePop();
					ImGui::PopID();
				}

				ImGui::Separator();
				if (ImGui::TreeNodeEx("Attach", ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::TreeNode("Component"))
					{
						static int current_item = 0;
						ImGui::Combo("Component", &current_item, "None\0Texture 2D\0Jinx Script\0Rigidbody 2D\0Camera Controller\0Light 2D\0Animation Contoller\0Audio Source\0\0");

						if (ImGui::Button("OK", ImVec2{ 60, 25 }))
						{
							switch (current_item)
							{
							case (uint32_t)ComponentItem::None:
							{
								break;
							}
							case (uint32_t)ComponentItem::AudioSource:
							{
								if (!m_SelectedActor->HasComponent<AudioSourceComponent>())
								{
									auto& ref = m_SelectedActor->AddComponent<AudioSourceComponent>();
									ref.AS = std::make_shared<AudioSource>();
								}

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
							case (uint32_t)ComponentItem::Animation2DContoller:
							{
								if (!m_SelectedActor->HasComponent<Animation2DControllerComponent>())
								{
									auto& ref = m_SelectedActor->AddComponent<Animation2DControllerComponent>();
									ref.AnimationController = std::make_shared<Animation2DController>();
								}

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
									m_SelectedActor->AddComponent<Rigidbody2DComponent>(m_SelectedActor, BodyType::Static);

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
							case (uint32_t)ComponentItem::Light2D:
							{
								if (!m_SelectedActor->HasComponent<Light2DComponent>())
								{
									m_SelectedActor->AddComponent<Light2DComponent>();
									break;
								}

								CONSOLE_WARN("Actor already has Light2D component");
								EDITOR_WARN("Actor <{}> already has Light2D component.", m_SelectedActor->GetName());
								break;
							}
							default:
								break;
							}

							current_item = 0;
						}
						ImGui::TreePop();
					}

					if (!m_SelectedActor->HasComponent<ScriptObject>())
					{
						if (ImGui::TreeNode("Script"))
						{
							static const char* current_item = m_Scene->m_ScriptNameList.front().c_str();

							if (ImGui::BeginCombo("Scripts", current_item))
							{
								for (int i = 0; i < m_Scene->m_ScriptNameList.size(); i++)
								{
									bool is_selected = (current_item == nullptr) ? false : (m_Scene->m_ScriptNameList[i].compare(current_item) == 0);
									if (ImGui::Selectable(m_Scene->m_ScriptNameList[i].c_str(), is_selected))
									{
										current_item = m_Scene->m_ScriptNameList[i].c_str();
									}

									if (is_selected)
									{
										ImGui::SetItemDefaultFocus();
									}
								}

								ImGui::EndCombo();
							}

							ImGui::Separator();
							ImGui::PushID("ScriptOKButton");
							if (ImGui::Button("OK", ImVec2{ 60, 25 }))
							{
								m_Scene->AttachScript(std::string(current_item), m_SelectedActor);
							}
							ImGui::PopID();

							ImGui::TreePop();
						}
					}

					ImGui::TreePop();
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();

		m_FileBrowser->Display();

		if (m_FileBrowser->HasSelected())
		{
			m_FilePath = m_FileBrowser->GetSelected().u8string();
			m_FileName = m_FileBrowser->GetSelected().filename().u8string();

			switch (m_FileBrowserState)
			{
			case FileBrowserFlags::ScriptPath:

				m_SelectedActor->AddComponent<JinxScriptComponent>(m_Actor, m_Scene->GetJinxRuntime(), m_FilePath);
				ResetFileBrowser();
				break;

			case FileBrowserFlags::Texture2dPath:
			{
				auto& texture = m_SelectedActor->GetComponent<Texture2DComponent>();
				
				texture.TexturePath = m_FilePath;
				texture.FileName = m_FileName;
				texture.Texture = Texture2D::Create(m_FilePath);
				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::SceneCreate:
			{
				m_FileBrowser = std::make_unique<ImGui::FileBrowser>();
				m_SelectedActor = nullptr;
				m_Scene->CreateScene(m_FilePath, m_FileName);
				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::SceneSave:
			{
				m_FileBrowser = std::make_unique<ImGui::FileBrowser>();

				m_Scene->GetSceneData().m_filePath = m_FilePath;
				m_Scene->GetSceneData().m_fileName = m_FileName;

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
			case FileBrowserFlags::LoadAnimationClip:
			{
				m_AnimationPanel->Load(m_FilePath);
				ResetFileBrowser();
				showAnimationPanel = true;
				break;
			}
			case FileBrowserFlags::LoadAudioClip:
			{
				auto& ac = m_SelectedActor->GetComponent<AudioSourceComponent>();
				auto clip = std::make_shared<AudioClip>();

				std::stringstream ss;
				ss << "New Audio Clip #" << ac.AS->m_AudioClips.size();

				clip->FileName = m_FileName;
				clip->FilePath = m_FilePath;
				clip->ClipName = ss.str();

				ac.AS->AddClip(clip);
				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::LoadClipController:
			{
				auto& controller = m_SelectedActor->GetComponent<Animation2DControllerComponent>();
				controller.AnimationController->LoadClip(m_FilePath);
				ResetFileBrowser();
				break;
			}
			default:
				break;
			}
		}


		ImGui::End();

	}

	void EditorLayer::ResetFileBrowser()
	{
		m_FileBrowser->ClearSelected();
		m_FilePath = "";
		m_FileName = "";
		m_FileBrowserState = FileBrowserFlags::None;
	}
}