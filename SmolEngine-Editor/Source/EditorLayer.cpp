#include "stdafx.h"
#include "../../GameX/CppScriptingExamples.h"

#include "EditorLayer.h"
#include "Core/ImGui/ImGuiExtension.h"
#include "icon_font_cpp_headers/IconsFontAwesome5.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "Core/Renderer/Renderer2D.h"
#include "Core/Animation/AnimationClip.h"
#include "Core/ECS/WorldAdmin.h"
#include "Core/ECS/Actor.h"

#include "Core/Audio/AudioClip.h"
#include "Core/Audio/AudioSource.h"

#include "Core/UI/UIButton.h"
#include "Core/UI/UITextLabel.h"
#include "Core/Renderer/Text.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <fstream>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include "Core/ImGui/NodeEditor/imnodes.h"


#ifndef SMOLENGINE_OPENGL_IMPL
#include "Core/Renderer/Vulkan/Vulkan.h"
#include "Core/Renderer/Vulkan/VulkanContext.h"
#include "Core/Renderer/Vulkan/VulkanPipelineSpecification.h"
#endif


namespace SmolEngine
{
	std::string EditorLayer::m_TempActorName = "";

	std::string EditorLayer::m_TempActorTag = "";

	std::string EditorLayer::m_TempString = "";

	///

	void EditorLayer::OnAttach()
	{
		m_FileBrowser = std::make_shared<ImGui::FileBrowser>();

		m_BuildPanel = std::make_unique<BuildPanel>();
		m_AnimationPanel = std::make_unique<AnimationPanel>();
		m_SettingsWindow = std::make_unique<SettingsWindow>();
		m_ActorCreationWindow = std::make_unique<ActorCreationWindow>();

		m_EditorConsole = EditorConsole::GetConsole();

		m_Scene = WorldAdmin::GetScene();
		m_Scene->CreateScene(std::string("C:/Dev/SmolEngine/SmolEngine-Editor/TestScene2.smolscene"), std::string("TestScene.smolscene"));

		auto Texture = Texture2D::Create("../GameX/Assets/Textures/bulkhead-wallsx3.png");

		auto camera = m_Scene->CreateActor(ActorBaseType::CameraBase, "Camera", "Default");

		auto actor = m_Scene->CreateActor(ActorBaseType::PhysicsBase, "Pawn");

		//actor->GetDefaultBaseTuple()->Light2D.isEnabled = true;
		//actor->GetDefaultBaseTuple()->Light2D.Color = { 0.8f, 0.6f, 0.4f, 1.0f };


	   // m_Scene->AddTuple<ResourceTuple>(*actor);
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
		m_Scene->UpdateEditorCamera(m_GameViewPortSize, m_SceneViewSize);
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
		//---------------------------------------WINDOW-STATES----------------------------------------//

		static bool showActorCreationWindow;
		static bool showRenderer2Dstats;
		static bool showConsole = true;
		static bool showGameView = false;
		static bool showSettingsWindow = false;
		static bool showBuildPanel = false;
		static bool showAnimationPanel = false;

		//TEMP

		static bool showNodeEditorTest = false;

		//---------------------------------------WINDOW-STATES----------------------------------------//

		static bool p_open = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

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

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 10.0f });
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File")) 
			{
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
						m_FileBrowserState = FileBrowserFlags::Scene_Create;
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
						m_FileBrowserState = FileBrowserFlags::Scene_Save;
						m_FileBrowser->Open();
					}

					if (ImGui::MenuItem("Load"))
					{
						m_SelectedActor = nullptr;

						m_FileBrowser->SetTypeFilters({ ".smolscene" });
						m_FileBrowser->SetTitle("Load Scene");
						m_FileBrowserState = FileBrowserFlags::Scene_Load;
						m_FileBrowser->Open();
					}
				}

				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("Create"))
			{
				if (!m_Scene->m_InPlayMode)
				{
					if (ImGui::MenuItem("New Actor"))
					{
						showActorCreationWindow = true;
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Animation"))
			{
				if (ImGui::MenuItem("New Clip"))
				{
					showAnimationPanel = true;
					m_AnimationPanel->m_AnimationClip = std::make_unique<AnimationClip>();
				}

				if (ImGui::MenuItem("Load Clip"))
				{
					m_FileBrowserState = FileBrowserFlags::Load_Animation_Clip;

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

				if (ImGui::MenuItem("Windows"))
				{
					m_BuildPanel->Load("../Config/ProjectConfig.smolconfig");
					showBuildPanel = true;
				}

				if (ImGui::MenuItem("Linux"))
				{

				}

				if (ImGui::MenuItem("Android"))
				{

				}

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

		m_BuildPanel->Update(showBuildPanel);

		m_SettingsWindow->Update(showSettingsWindow, m_Scene);

		m_ActorCreationWindow->Update(showActorCreationWindow, m_Scene);

		m_AnimationPanel->Update(showAnimationPanel);

		m_EditorConsole->Update(showConsole);

		if (showRenderer2Dstats)
		{
			if (ImGui::Begin("Renderer2D: Stats", &showRenderer2Dstats))
			{
				ImGui::Extensions::Text("Squares", std::to_string(Renderer2D::Stats->QuadCount));

				ImGui::Extensions::Text("Vertices", std::to_string(Renderer2D::Stats->GetTotalVertexCount()));

				ImGui::Extensions::Text("Indices", std::to_string(Renderer2D::Stats->GetTotalIndexCount()));

				ImGui::Extensions::Text("Textures", std::to_string(Renderer2D::Stats->TexturesInUse));

				ImGui::Extensions::Text("Layers", std::to_string(Renderer2D::Stats->LayersInUse) + " / 10");

				ImGui::NewLine();

				ImGui::Extensions::Text("Draw Calls", std::to_string(Renderer2D::Stats->DrawCalls));

				ImGui::End();
			}
		}

		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
			ImGui::Begin("Scene View");
			{

				if (ImGui::IsWindowFocused()) { isSceneViewFocused = true; }
				else { isSceneViewFocused = false; }

				m_SceneViewSize = { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };

				auto& frameBuffer = m_Scene->m_EditorCamera->m_FrameBuffer;
				ImVec2 ViewPortSize = ImGui::GetContentRegionAvail();

				if (ViewPortSize.x != m_ViewPortSize.x || ViewPortSize.y != m_ViewPortSize.y)
				{
					m_ViewPortSize = { ViewPortSize.x, ViewPortSize.y };
					m_Scene->OnSceneViewResize(m_ViewPortSize.x, m_ViewPortSize.y);
				}

#ifdef SMOLENGINE_OPENGL_IMPL

				ImGui::Image(frameBuffer->GetImGuiTextureID(), ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2(0, 1), ImVec2(1, 0));
#else
				ImGui::Image(frameBuffer->GetImGuiTextureID(), ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y });
#endif // SMOLENGINE_OPENGL_IMPL

			}
			ImGui::End();
			ImGui::PopStyleVar();
		}

		if (showGameView)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
			ImGui::Begin("Game View", &showGameView);
			{
				ImGui::SetWindowSize("Game View", { 720.0f, 480.0f });

				m_GameViewSize = { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };

				if (ImGui::IsWindowFocused()) { isGameViewFocused = true; }
				else { isGameViewFocused = false; }

				ImVec2 ViewPortSize = ImGui::GetContentRegionAvail();

				if (ViewPortSize.x != m_GameViewPortSize.x || ViewPortSize.y != m_GameViewPortSize.y)
				{
					m_GameViewPortSize = { ViewPortSize.x, ViewPortSize.y };
					m_Scene->OnGameViewResize(m_GameViewPortSize.x, m_GameViewPortSize.y);
				}

				auto framebuffer = FramebufferSComponent::Get()[0];
				if (framebuffer)
				{

#ifdef SMOLENGINE_OPENGL_IMPL
					ImGui::Image(framebuffer->GetImGuiTextureID(),
						ImVec2{ m_GameViewPortSize.x, m_GameViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
#else
					ImGui::Image(framebuffer->GetImGuiTextureID(),
						ImVec2{ m_GameViewPortSize.x, m_GameViewPortSize.y });
#endif
				}
			}

			ImGui::End();
			ImGui::PopStyleVar();

		}

		// TEMP

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
					for (const auto& actor : m_Scene->GetSortedActorList())
					{
						if (ImGui::TreeNodeEx(actor->GetName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow))
						{

							ImGui::TreePop();
						}


						if (ImGui::IsItemClicked(1))
						{
							m_SelectionFlags = SelectionFlags::Actions;

							m_TempActorTag = "";
							m_TempActorName = "";

							m_SelectedActor = nullptr;
							m_SelectedActor = actor;
						}

						if (ImGui::IsItemClicked())
						{
							m_SelectionFlags = SelectionFlags::Inspector;
							if (m_SelectedActor != nullptr)
							{
								m_SelectedActor->m_showComponentUI = false;
								m_SelectedActor = nullptr;
							}

							m_TempActorTag = "";
							m_TempActorName = "";

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


					if (ImGui::MenuItem("Dublicate", "Ctrl+C"))
					{

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
				std::stringstream ss;

				switch (m_SelectedActor->ActorType)
				{
				case ActorBaseType::DefaultBase:
				{
					ss << "Default Actor";

					ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.6f);
					ImGui::TextUnformatted(ss.str().c_str());

					auto ref = m_SelectedActor->GetDefaultBaseTuple();

					ImGui::Separator();
					ImGui::NewLine();

					// Head

					if (ImGui::CollapsingHeader("Head", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();

						{
							DrawInfo(&ref->Info);
						}
					}

					ImGui::NewLine();

					// Transform 

					if (ImGui::CollapsingHeader("Tranform", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();
						DrawTransform(&ref->Transform);
					}

					ImGui::NewLine();

					// Texture2D

					if (ImGui::CollapsingHeader("Texture2D", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();

						DrawTexture(&ref->Texture);
					}

					ImGui::NewLine();

					// Light2D

					if (ImGui::CollapsingHeader("Light 2D", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();

						DrawLight2D(&ref->Light2D);
					}

					ImGui::NewLine();

					break;
				}
				case ActorBaseType::PhysicsBase:
				{
					ss << "Physics Actor";

					ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.6f);
					ImGui::TextUnformatted(ss.str().c_str());

					auto ref = m_SelectedActor->GetPhysicsBaseTuple();

					ImGui::Separator();
					ImGui::NewLine();

					// Head

					if (ImGui::CollapsingHeader("Head", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();

						{
							DrawInfo(&ref->Info);
						}
					}

					ImGui::NewLine();

					// Transform 

					if (ImGui::CollapsingHeader("Tranform", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();
						DrawTransform(&ref->Transform);
					}

					ImGui::NewLine();

					// Texture2D

					if (ImGui::CollapsingHeader("Texture2D", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();

						DrawTexture(&ref->Texture);
					}

					ImGui::NewLine();

					// Light2D

					if (ImGui::CollapsingHeader("Light 2D", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();

						DrawLight2D(&ref->Light2D);
					}

					ImGui::NewLine();

					// Body2D

					if (ImGui::CollapsingHeader("Rigidbody2D", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();

						DrawBody2D(&ref->Body);

					}

					ImGui::NewLine();

					break;
				}
				case ActorBaseType::CameraBase:
				{
					ss << "Camera Actor";

					ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.6f);
					ImGui::TextUnformatted(ss.str().c_str());

					auto ref = m_SelectedActor->GetCameraBaseTuple();

					ImGui::Separator();
					ImGui::NewLine();

					// Head

					if (ImGui::CollapsingHeader("Head", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();

						{
							DrawInfo(&ref->Info);
						}
					}

					ImGui::NewLine();

					// Transform 

					if (ImGui::CollapsingHeader("Tranform", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();
						DrawTransform(&ref->Transform);
					}

					ImGui::NewLine();

					// Camera

					if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();

						DrawCamera(&ref->Camera);
					}

					ImGui::NewLine();

					// Canvas

					if (ImGui::CollapsingHeader("Canvas", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();

						DrawCanvas(&ref->Canvas);
					}

					ImGui::NewLine();

					break;
				}
				default:
					break;
				}

				if (m_Scene->HasTuple<ResourceTuple>(*m_SelectedActor))
				{
					auto reTuple = m_Scene->GetTuple<ResourceTuple>(*m_SelectedActor);

					if (ImGui::CollapsingHeader("Animation 2D", ImGuiTreeNodeFlags_DefaultOpen))
					{
						DrawAnimation2D(&reTuple->Animation2D);

					}

					ImGui::NewLine();

					if (ImGui::CollapsingHeader("Audio Source", ImGuiTreeNodeFlags_DefaultOpen))
					{
						DrawAudioSource(&reTuple->AudioSource);
					}

					ImGui::NewLine();
				}

				if (m_Scene->HasTuple<BehaviourComponent>(*m_SelectedActor))
				{
					auto behaviourRef = m_Scene->GetTuple<BehaviourComponent>(*m_SelectedActor);

					if (ImGui::CollapsingHeader(behaviourRef->SystemName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::NewLine();

						ImGui::Extensions::Text("System Type", "C++ Script");

						ImGui::NewLine();

						DrawBehaviorComponent(behaviourRef);
					}

					ImGui::NewLine();
				}

				ImGui::Separator();
				ImGui::NewLine();

				if (!m_SelectedActor->m_showComponentUI)
				{
					if (ImGui::Button("Add System", { ImGui::GetWindowSize().x - 10.0f, 30 }))
					{
						m_SelectedActor->m_showComponentUI = true;
					}

				}
				else
				{
					std::vector<std::string> list;

					uint16_t actorType = (uint16_t)m_SelectedActor->ActorType;

					if (m_SelectedActor->ActorType != ActorBaseType::CameraBase)
					{
						list.push_back("Resource System");
					}

					for (const auto& pair : SystemRegistry::Get()->m_SystemMap)
					{
						const auto& [name, type] = pair;
						if (actorType == type)
						{
							list.push_back(name);
						}
					}

					static std::string name;
					ImGui::PushID("SystemSearch");
					ImGui::Extensions::InputRawString("Search", name, "System");
					ImGui::PopID();
					ImGui::NewLine();

					ImGui::BeginChild("SystemText", { ImGui::GetWindowSize().x - 30.0f, 200 }, ImGuiWindowFlags_AlwaysVerticalScrollbar);
					for (auto str : list)
					{
						auto result = str.find(name);
						if (result == std::string::npos)
						{
							continue;
						}

						bool selected = false;

						if (ImGui::Selectable(str.c_str(), selected))
						{
							name = str;
						}

						if (ImGui::IsItemClicked(0))
						{
							selected = true;
						}
					}

					ImGui::EndChild();

					ImGui::NewLine();

					if (ImGui::Button("Add", { ImGui::GetWindowSize().x - 30.0f, 30 }))
					{
						if (name == "Resource System")
						{
							m_Scene->AddTuple<ResourceTuple>(*m_SelectedActor);
						}
						else
						{
							m_Scene->AddBehaviour(name, m_SelectedActor);
						}

						name = "";
						m_SelectedActor->m_showComponentUI = false;
					}

				}
			}

			ImGui::NewLine();
			ImGui::NewLine();

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
			case FileBrowserFlags::Load_Jinx_Script:

				ResetFileBrowser();
				break;

			case FileBrowserFlags::Load_Texture2D:
			{
				switch (m_SelectedActor->ActorType)
				{
				case ActorBaseType::DefaultBase:
				{
					auto ref = m_SelectedActor->GetDefaultBaseTuple();

					m_Scene->DeleteAsset(ref->Texture.FileName);

					ref->Texture.FileName = m_FileName;
					ref->Texture.Texture = Texture2D::Create(m_FilePath);

					m_Scene->AddAsset(m_FileName, m_FilePath);

					break;
				}
				case ActorBaseType::PhysicsBase:
				{
					auto ref = m_SelectedActor->GetPhysicsBaseTuple();

					m_Scene->DeleteAsset(ref->Texture.FileName);

					ref->Texture.FileName = m_FileName;
					ref->Texture.Texture = Texture2D::Create(m_FilePath);

					m_Scene->AddAsset(m_FileName, m_FilePath);

					break;
				}
				default:

					break;
				}

				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Scene_Create:
			{
				m_FileBrowser = std::make_unique<ImGui::FileBrowser>();

				m_SelectedActor = nullptr;
				m_Scene->CreateScene(m_FilePath, m_FileName);

				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Scene_Save:
			{
				m_FileBrowser = std::make_unique<ImGui::FileBrowser>();

				m_Scene->GetSceneData().m_filePath = m_FilePath;
				m_Scene->GetSceneData().m_fileName = m_FileName;
				m_Scene->Save(m_FilePath);

				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Scene_Load:
			{
				m_SelectedActor = nullptr;

				m_Scene->Load(m_FilePath);

				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Load_Animation_Clip:
			{
				m_AnimationPanel->Load(m_FilePath);
				ResetFileBrowser();
				showAnimationPanel = true;

				break;
			}
			case FileBrowserFlags::Load_Audio_Clip:
			{
				auto clip = std::make_shared<AudioClip>();
				auto tuple = m_Scene->GetTuple<ResourceTuple>(*m_SelectedActor);

				if (tuple != nullptr)
				{
					std::stringstream ss;
					ss << "New Audio Clip #" << tuple->AudioSource.AudioClips.size();

					clip->FileName = m_FileName;
					clip->FilePath = m_FilePath;
					clip->ClipName = ss.str();

					if (AudioSystem::AddClip(tuple->AudioSource, clip))
					{
						m_Scene->AddAsset(m_FileName, m_FilePath);
					}
				}

				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Canvas_Chanage_Button_Texture:
			{
				auto camera = m_SelectedActor->GetCameraBaseTuple();
				if (camera != nullptr)
				{
					auto button = UISystem::GetButton(camera->Canvas, m_IDBuffer);
					if (button != nullptr)
					{
						m_Scene->DeleteAsset(button->m_TetxureName);

						m_Scene->AddAsset(m_FileName, m_FilePath);
						button->Init(m_FilePath, m_FileName);
					}
				}

				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Canavas_Create_TextLabel:
			{
				auto camera = m_SelectedActor->GetCameraBaseTuple();
				if (camera != nullptr)
				{
					auto element = UISystem::AddElement(camera->Canvas, UIElementType::TextLabel);
					auto textLabel = std::static_pointer_cast<UITextLabel>(element);

					m_Scene->AddAsset(m_FileName, m_FilePath);
					textLabel->Init(m_FilePath, m_FileName);
				}

				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Canavas_TextLabel_Load_Font:
			{
				auto camera = m_SelectedActor->GetCameraBaseTuple();
				if (camera != nullptr)
				{
					auto text = UISystem::GetTextLabel(camera->Canvas, m_IDBuffer);
					if (text != nullptr)
					{
						m_Scene->DeleteAsset(text->m_FontName);

						text->m_FontFilePath = m_FilePath;
						text->m_FontName = m_FileName;
						text->SetFont(m_FilePath);

						m_Scene->AddAsset(m_FileName, m_FilePath);
					}
				}

				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Load_Animation_Clip_Inspector:
			{
				auto resRef = m_Scene->GetTuple<ResourceTuple>(m_SelectedActor->Entity);
				if (resRef)
				{
					if (Animation2DSystem::LoadClip(resRef->Animation2D, m_FilePath))
					{
						m_Scene->AddAsset(m_FileName, m_FilePath);
					}
				}

				ResetFileBrowser();
				break;
			}
			default:
				break;
			}
		}


		ImGui::End();

	}

	void EditorLayer::DrawInfo(HeadComponent* head)
	{
		m_TempActorName = head->Name;
		m_TempActorTag = head->Tag;

		if (ImGui::Extensions::InputRawString("Name", m_TempActorName))
		{
			if (m_Scene->OnActorNameChanged(head->Name, m_TempActorName))
			{
				head->Name = m_TempActorName;
			}
		}

		ImGui::Extensions::InputString("Tag", head->Tag, m_TempActorTag);

		ImGui::NewLine();
		ImGui::Extensions::CheckBox("Is Enabled?", head->IsEnabled, 130.0f, "HeadPanel");
	}

	void EditorLayer::DrawTransform(TransformComponent* transform)
	{
		ImGui::Extensions::TransformComponent(transform->WorldPos, transform->Scale, transform->Rotation);
	}

	void EditorLayer::DrawTexture(Texture2DComponent* texture)
	{
		if (texture->Texture != nullptr)
		{
			ImGui::Extensions::Texture("Texture", texture->Texture->GetImGuiTexture());

			ImGui::NewLine();
			ImGui::Extensions::ColorInput3("Color", texture->Color);

			ImGui::Extensions::InputInt("Layer", texture->LayerIndex, 130.0f, "TexturePanel");

			ImGui::NewLine();
			if (ImGui::Extensions::CheckBox("Enabled?", texture->Enabled, 130.0f, "TexturePanel")) 

			ImGui::NewLine();
			if (ImGui::Button("Change", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
			{
				m_FileBrowserState = FileBrowserFlags::Load_Texture2D;
				m_FileBrowser->SetTitle("Select a texture");
				m_FileBrowser->SetTypeFilters({ ".png" });
				m_FileBrowser->Open();
			}
		}
		else
		{
			ImGui::NewLine();

			if (ImGui::Button("New texture", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
			{
				m_FileBrowserState = FileBrowserFlags::Load_Texture2D;
				m_FileBrowser->SetTitle("Select a texture");
				m_FileBrowser->SetTypeFilters({ ".png" });
				m_FileBrowser->Open();
			}
		}

	}

	void EditorLayer::DrawBody2D(Body2DComponent* rb)
	{
		ImGui::Extensions::Combo("Type", "Static\0Kinematic\0Dynamic\0\0", rb->Body.m_Type);
		ImGui::Extensions::Combo("Shape", "Box\0Circle\0\0", rb->Body.m_ShapeType);
		ImGui::Extensions::InputInt("Layer", rb->Body.m_CollisionLayer);

		ImGui::NewLine();

		if (rb->Body.m_ShapeType == (int)ShapeType::Box)
		{
			ImGui::Extensions::InputFloat2Base("Size", rb->Body.m_Shape);

			ImGui::NewLine();
		}

		if (rb->Body.m_ShapeType == (int)ShapeType::Cirlce)
		{
			ImGui::Extensions::InputFloat("Radius", rb->Body.m_Radius);
			ImGui::Extensions::InputFloat2Base("Offset", rb->Body.m_Offset);

			ImGui::NewLine();
		}


		if (rb->Body.m_Type == 2 || rb->Body.m_Type == 1)
		{
			ImGui::Extensions::InputFloat("Inertia Moment", rb->Body.m_InertiaMoment);
			ImGui::Extensions::InputFloat("Gravity", rb->Body.m_GravityScale);
			ImGui::Extensions::InputFloat("Mass", rb->Body.m_Mass);
			ImGui::Extensions::InputFloat2Base("Mass Center", rb->Body.m_MassCenter);

			ImGui::NewLine();

			ImGui::Extensions::InputFloat("Restitution", rb->Body.m_Restitution);
			ImGui::Extensions::InputFloat("Friction", rb->Body.m_Friction);
			ImGui::Extensions::InputFloat("Density", rb->Body.m_Density);
			ImGui::Extensions::CheckBox("Is Bullet?", rb->Body.m_IsBullet);

			ImGui::NewLine();

		}

		ImGui::Extensions::CheckBox("Is Trigger?", rb->Body.m_IsTrigger);
		ImGui::Extensions::CheckBox("Is Awake?", rb->Body.m_IsAwake);
		ImGui::Extensions::CheckBox("Allow Sleep?", rb->Body.m_canSleep);
		ImGui::Extensions::CheckBox("Draw Shape?", rb->ShowShape);

	}

	void EditorLayer::DrawCamera(CameraComponent* camera)
	{
		ImGui::Extensions::InputFloat("Zoom", camera->ZoomLevel);
		ImGui::Extensions::InputFloat("Near", camera->zNear);
		ImGui::Extensions::InputFloat("Far", camera->zFar);

		ImGui::NewLine();

		if(ImGui::Extensions::CheckBox("Is Primary?", camera->isPrimaryCamera))
		{
			if (camera->isPrimaryCamera)
			{
				const size_t id = m_SelectedActor->GetID();

				m_Scene->m_SceneData.m_Registry.view<CameraBaseTuple>().each([&](CameraBaseTuple& tuple)
				{
					if (tuple.Info.ID != id)
					{
						tuple.Camera.isPrimaryCamera = false;
					}
				});
			}
		}

		ImGui::Extensions::CheckBox("Is Enabled?", camera->isEnabled);

		ImGui::Extensions::CheckBox("Show Shape?", camera->ShowCanvasShape);
	}

	void EditorLayer::DrawAudioSource(AudioSourceComponent* audio)
	{
		for (auto& pair: audio->AudioClips)
		{
			auto& [key, clip] = pair;

			// Tree

			ImGui::NewLine();
			std::stringstream ss;
			ss << "Audio Clip #" << clip->ClipName;

			if (ImGui::TreeNodeEx(ss.str().c_str()))
			{
				ImGui::NewLine();

				if (ImGui::Extensions::InputRawString("Key Name", m_TempString))
				{
					if (!m_TempString.empty())
					{
						AudioSystem::RenameClip(*audio, clip->ClipName, m_TempString);
					}

					ImGui::TreePop();
					break;
				}

				ImGui::NewLine();

				ImGui::Extensions::InputFloat("Volume", clip->Volume);

				ImGui::NewLine();

				ImGui::Extensions::CheckBox("Default Clip?", clip->isDefaultClip);
				ImGui::Extensions::CheckBox("Is Looping?", clip->IsLooping);
				ImGui::Extensions::CheckBox("Is 3D?", clip->B3D);

				ImGui::NewLine();

				if (ImGui::Extensions::SmallButton("Debug", "Play"))
				{
					AudioSystem::DebugPlay(clip, AudioEngineSComponent::Get()->Engine);
				}

				ImGui::SameLine();

				if (ImGui::SmallButton("Stop"))
				{
					AudioSystem::DebugStop(clip, AudioEngineSComponent::Get()->Engine);
				}

				ImGui::TreePop();
			}

			ImGui::Separator();
			m_TempString = "";

		}

		ImGui::NewLine();
		ImGui::NewLine();

		if (ImGui::Button("New Audio Clip", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
		{
			m_FileBrowserState = FileBrowserFlags::Load_Audio_Clip;
			m_FileBrowser->SetTitle("Select audio clip");
			m_FileBrowser->SetTypeFilters({ ".wav" });
			m_FileBrowser->Open();
		}
	}

	void EditorLayer::DrawAnimation2D(Animation2DComponent* anim)
	{
		for (auto& pair : anim->m_Clips)
		{
			auto& [key, clip] = pair;

			// Tree

			ImGui::NewLine();

			ImGui::Extensions::InputInt("Layer", anim->IndexLayer, 130.0f, "AnimationPanel");

			ImGui::NewLine();

			std::stringstream ss;
			ss << "Animation Clip #" << clip->m_ClipName;


			if(ImGui::TreeNodeEx(ss.str().c_str()))
			{
				ImGui::NewLine();

				if (ImGui::Extensions::InputRawString("Key Name", m_TempString))
				{
					if (!m_TempString.empty())
					{
						Animation2DSystem::RenameClip(*anim, clip->m_ClipName, m_TempString);
					}

					ImGui::TreePop();
					break;
				}

				ImGui::Extensions::CheckBox("Default Clip?", clip->m_IsDefaultClip);

				ImGui::NewLine();
				
				if (ImGui::Extensions::SmallButton("Debug", "Play"))
				{
					Animation2DSystem::Play(clip->m_ClipName, *anim);
				}

				ImGui::SameLine();

				if (ImGui::SmallButton("Stop"))
				{
					Animation2DSystem::Stop(clip->m_ClipName, *anim);
				}

				ImGui::TreePop();
			}

			ImGui::Separator();
			m_TempString = "";
		}

		ImGui::NewLine();
		ImGui::NewLine();

		if (ImGui::Button("New Animation Clip", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
		{
			m_FileBrowserState = FileBrowserFlags::Load_Animation_Clip_Inspector;
			m_FileBrowser->SetTitle("Select Animation2D File");
			m_FileBrowser->SetTypeFilters({ ".smolanim" });
			m_FileBrowser->Open();
		}
	}

	void EditorLayer::DrawCanvas(CanvasComponent* canvas)
	{
		for (const auto& pair : canvas->Elements)
		{
			const auto& [key, element] = pair;

			ImGui::NewLine();
			std::stringstream ss;
			
			switch (element->m_Type)
			{
			case UIElementType::TextLabel:
			{
				ss << "TextLabel #" << element->m_ID;

				if (ImGui::TreeNodeEx(ss.str().c_str()))
				{
					ImGui::NewLine();

					Ref<UITextLabel> textLabel = std::static_pointer_cast<UITextLabel>(element);

					if (ImGui::Extensions::InputRawString("Text", textLabel->m_Text, "Text"))
					{
						textLabel->SetText(textLabel->m_Text);
					}

					ImGui::NewLine();

					ImGui::Extensions::InputFloat2Base("Position", textLabel->m_Position);
					ImGui::Extensions::InputFloat2Base("Size", textLabel->m_Size);

					ImGui::Extensions::ColorInput3("Color", textLabel->m_Color);
					ImGui::Extensions::InputFloat("Padding", textLabel->m_Padding);

					ImGui::NewLine();

					if (ImGui::Button("New Font", { ImGui::GetWindowWidth() - 60.0f, 30.0f }))
					{
						m_IDBuffer = textLabel->m_ID;

						m_FileBrowserState = FileBrowserFlags::Canavas_TextLabel_Load_Font;
						m_FileBrowser->SetTitle("Select a font");
						m_FileBrowser->SetTypeFilters({ ".ttf" });
						m_FileBrowser->Open();
					}

					ImGui::NewLine();
					ImGui::Separator();
					ImGui::TreePop();
				}


				break;
			}
			case UIElementType::Button:
			{
				ss << "Button #" << element->m_ID;

				if (ImGui::TreeNodeEx(ss.str().c_str()))
				{
					ImGui::NewLine();

					Ref<UIButton> button = std::static_pointer_cast<UIButton>(element);

					if (button->m_Texture != nullptr)
					{
						ImGui::Extensions::Texture("Texture", button->m_Texture->GetImGuiTexture());

						ImGui::Extensions::ColorInput3("Normal Color", button->m_CurrentColor);
						ImGui::Extensions::ColorInput3("Hovered Color", button->m_HoveredColor);
						ImGui::Extensions::ColorInput3("Pressed Color", button->m_PressedColor);

						ImGui::NewLine();

						ImGui::Extensions::InputFloat2Base("Position", button->m_UCood);
						ImGui::Extensions::InputFloat2Base("Size", button->m_Size);

						ImGui::NewLine();

						if (ImGui::Button("New Texture", { ImGui::GetWindowWidth() - 60.0f, 30.0f }))
						{
							m_IDBuffer = element->m_ID;

							m_FileBrowserState = FileBrowserFlags::Canvas_Chanage_Button_Texture;
							m_FileBrowser->SetTitle("Select a texture");
							m_FileBrowser->SetTypeFilters({ ".png" });
							m_FileBrowser->Open();
						}
					}
					else
					{
						if (ImGui::Button("Set Button Texture", { ImGui::GetWindowWidth() - 60.0f, 30.0f }))
						{
							m_IDBuffer = element->m_ID;

							m_FileBrowserState = FileBrowserFlags::Canvas_Chanage_Button_Texture;
							m_FileBrowser->SetTitle("Select a texture");
							m_FileBrowser->SetTypeFilters({ ".png" });
							m_FileBrowser->Open();
						}
					}

					ImGui::NewLine();
					ImGui::Separator();
					ImGui::TreePop();
				}

				break;
			}
			default:
				break;
			}
		}

		ImGui::NewLine();

		if (ImGui::Extensions::SmallButton("Create", "TextLabel"))
		{
			m_FileBrowserState = FileBrowserFlags::Canavas_Create_TextLabel;
			m_FileBrowser->SetTitle("Select a font");
			m_FileBrowser->SetTypeFilters({ ".ttf" });
			m_FileBrowser->Open();
		}

		ImGui::SameLine();

		if (ImGui::SmallButton("Button"))
		{
			UISystem::AddElement(*canvas, UIElementType::Button);
		}
	}

	void EditorLayer::DrawBehaviorComponent(BehaviourComponent* behaviour)
	{
		for (auto& val : behaviour->OutValues)
		{
			switch (val.Value.index())
			{

			case (uint32_t)OutValueType::Float:
			{
				ImGui::Extensions::InputFloat(val.Key.c_str(), std::get<float>(val.Value));

				break;
			}
			case (uint32_t)OutValueType::Int:
			{
				ImGui::Extensions::InputInt(val.Key.c_str(), std::get<int>(val.Value));

				break;
			}
			case (uint32_t)OutValueType::String:
			{
				ImGui::Extensions::InputRawString(val.Key.c_str(), std::get<std::string>(val.Value), val.Key.c_str());

				break;
			}
			default:
				break;
			}
		}
	}

	void EditorLayer::DrawLight2D(Light2DSourceComponent* light)
	{
		ImGui::Extensions::InputFloat2Base("Offset", light->Position, 130.0f, "2DLightPanel");
		ImGui::Extensions::InputFloat("Intensity", light->Intensity, 130.0f, "2DLightPanel");
		ImGui::Extensions::InputFloat("Radius", light->Radius, 130.0f, "2DLightPanel");
		ImGui::Extensions::ColorInput3("Color", light->Color, 130.0f, "2DLightPanel");

		ImGui::NewLine();

		ImGui::Extensions::CheckBox("Is Enabled?", light->isEnabled, 130.0f, "2DLightPanel");
	}

	void EditorLayer::ResetFileBrowser()
	{
		m_FileBrowser->ClearSelected();
		m_FilePath = "";
		m_FileName = "";
		m_IDBuffer = 0;
		m_FileBrowserState = FileBrowserFlags::None;
	}
}