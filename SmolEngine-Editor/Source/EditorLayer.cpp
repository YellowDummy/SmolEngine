#include "stdafx.h"

#include "EditorLayer.h"

#include "Renderer/Renderer2D.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Text.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/MaterialLibrary.h"
#ifndef SMOLENGINE_OPENGL_IMPL
#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/VulkanContext.h"
#endif
#include "Renderer/Mesh.h"

#include "Animation/AnimationClip2D.h"

#include "ECS/WorldAdmin.h"
#include "ECS/Actor.h"
#include "ECS/ComponentsCore.h"
#include "ECS/Systems/RendererSystem.h"
#include "ECS/Systems/Physics2DSystem.h"
#include "ECS/Systems/AudioSystem.h"
#include "ECS/Systems/Animation2DSystem.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/CommandSystem.h"
#include "ECS/Systems/UISystem.h"
#include "ECS/Systems/ScriptingSystem.h"
#include "ECS/Systems/CommandSystem.h"
#include "ECS/Components/Singletons/AudioEngineSComponent.h"
#include "ECS/Components/Singletons/Box2DWorldSComponent.h"
#include "ECS/Components/Singletons/ProjectConfigSComponent.h"
#include "ECS/Components/Singletons/JobsSystemStateSComponent.h"
#include "ECS/Scene.h"

#include "Audio/AudioClip.h"

#include "UI/UIButton.h"
#include "UI/UITextLabel.h"
#include "Core/Input.h"
#include "Core/FileDialog.h"
#include "Core/FilePaths.h"

#include "ImGui/ImGuiExtension.h"
#include "icon_font_cpp_headers/IconsFontAwesome5.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "ImGui/NodeEditor/imnodes.h"
#include "ImGui/ImPlot/implot.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <fstream>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace SmolEngine
{

	void EditorLayer::OnAttach()
	{
		GraphicsContextInitInfo GCInfo = {};
		GCInfo.bMSAA = true;
		GCInfo.bTargetsSwapchain = false;
		assert(GraphicsContext::Init(GCInfo) == true);

		//auto mesh = Mesh::Create(Resources + "Models/anim.glb");

		EditorCameraCreateInfo editorCamCI{};
		{
			editorCamCI.FOV = 65.5f;
			editorCamCI.WorldPos = glm::vec3(0, 0, -6);
			editorCamCI.Type = CameraType::Perspective;
			m_Camera = std::make_shared<EditorCamera>(&editorCamCI);
		}

		m_FileBrowser = std::make_shared<ImGui::FileBrowser>();
		m_BuildPanel = std::make_unique<BuildPanel>();
		m_AnimationPanel = std::make_unique<AnimationPanel>();
		m_SettingsWindow = std::make_unique<SettingsWindow>();
		m_MaterialLibraryInterface = std::make_unique<MaterialLibraryInterface>();

		m_EditorConsole = EditorConsole::GetConsole();
		m_Scene = WorldAdmin::GetSingleton();

		m_Scene->CreateScene(std::string("TestScene.s_scene"));

	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(DeltaTime deltaTime)
	{
		if (isSceneViewFocused)
			m_Camera->OnUpdate(deltaTime);

		m_Scene->UpdateEditorCamera(m_Camera);
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
			m_Camera->OnEvent(event);

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
		static bool showMaterialLibrary = false;

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
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
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
					Engine::GetEngine().Shutdown();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scene"))
			{
				if (!m_Scene->IsInPlayMode())
				{
					if (ImGui::MenuItem("New"))
					{
						const auto& result = FileDialog::SaveFile("SmolEngine Scene (*.s_scene)\0*.s_scene\0", "new_scene.s_scene");
						if (result.has_value())
						{
							m_SelectedActor = nullptr;
							m_Scene->CreateScene(result.value());
						}
					}

					if (ImGui::MenuItem("Save"))
					{
						if (!m_Scene->SaveCurrentScene())
						{
							CONSOLE_ERROR("Couldn't save current scene!");
						}
					}

					if (ImGui::MenuItem("Save as"))
					{
						const auto& result = FileDialog::SaveFile("SmolEngine Scene (*.s_scene)\0*.s_scene\0", "new_scene.s_scene");
						if (result.has_value())
						{
							m_SelectedActor = nullptr;
							m_Scene->GetActiveScene()->GetSceneData().m_filePath = result.value();
							std::filesystem::path path = result.value();
							m_Scene->GetActiveScene()->GetSceneData().m_Name = path.filename().stem().string();
							m_Scene->Save(result.value());
						}
					}

					if (ImGui::MenuItem("Load"))
					{
						const auto& result = FileDialog::OpenFile("SmolEngine Scene (*.s_scene)\0*.s_scene\0");
						if (result.has_value())
						{
							m_SelectedActor = nullptr;
							m_Scene->Load(result.value());
						}
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Animation"))
			{
				if (ImGui::MenuItem("New Clip"))
				{

				}

				if (ImGui::MenuItem("Load Clip"))
				{

				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Simulation"))
			{
				if (ImGui::MenuItem("Play Mode"))
				{
					if (!m_Scene->IsInPlayMode())
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
					if (m_Scene->IsInPlayMode())
					{
						uint32_t selectedActorID = 0;
						if (m_SelectedActor != nullptr)
						{
							selectedActorID = m_SelectedActor->GetID();
						}

						m_SelectedActor = nullptr;
						m_Scene->OnEndPlay();
						m_SelectedActor = m_Scene->GetActiveScene()->FindActorByID(selectedActorID);
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
					const std::string path = "../Config/ProjectConfig.smolconfig";
					m_BuildPanel->Load(path);
					showBuildPanel = true;
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

				if (ImGui::MenuItem("Material Library"))
				{
					showMaterialLibrary = true;
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

#if  0
		ImGui::Begin("My Window");
		{
			struct ScrollingBuffer {
				int MaxSize;
				int Offset;
				ImVector<ImVec2> Data;
				ScrollingBuffer(int max_size = 2000) {
					MaxSize = max_size;
					Offset = 0;
					Data.reserve(MaxSize);
				}
				void AddPoint(float x, float y) {
					if (Data.size() < MaxSize)
						Data.push_back(ImVec2(x, y));
					else {
						Data[Offset] = ImVec2(x, y);
						Offset = (Offset + 1) % MaxSize;
					}
				}
				void Erase() {
					if (Data.size() > 0) {
						Data.shrink(0);
						Offset = 0;
					}
				}
			};

			// utility structure for realtime plot
			struct RollingBuffer {
				float Span;
				ImVector<ImVec2> Data;
				RollingBuffer() {
					Span = 10.0f;
					Data.reserve(2000);
				}
				void AddPoint(float x, float y) {
					float xmod = fmodf(x, Span);
					if (!Data.empty() && xmod < Data.back().x)
						Data.shrink(0);
					Data.push_back(ImVec2(xmod, y));
				}
			};

			ImGui::BulletText("Move your mouse to change the data!");
			ImGui::BulletText("This example assumes 60 FPS. Higher FPS requires larger buffer size.");
			static ScrollingBuffer sdata1, sdata2;
			static RollingBuffer   rdata1, rdata2;
			ImVec2 mouse = ImGui::GetMousePos();
			static float t = 0;
			t += ImGui::GetIO().DeltaTime;
			sdata1.AddPoint(t, mouse.x * 0.0005f);
			rdata1.AddPoint(t, mouse.x * 0.0005f);
			sdata2.AddPoint(t, mouse.y * 0.0005f);
			rdata2.AddPoint(t, mouse.y * 0.0005f);

			static float history = 10.0f;
			ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");
			rdata1.Span = history;
			rdata2.Span = history;

			static ImPlotAxisFlags rt_axis = ImPlotAxisFlags_NoTickLabels;
			ImPlot::SetNextPlotLimitsX(t - history, t, ImGuiCond_Always);
			if (ImPlot::BeginPlot("##Scrolling", NULL, NULL, ImVec2(-1, 150), 0, rt_axis, rt_axis | ImPlotAxisFlags_LockMin)) {
				ImPlot::PlotShaded("Data 1", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), 0, sdata1.Offset, 2 * sizeof(float));
				ImPlot::PlotLine("Data 2", &sdata2.Data[0].x, &sdata2.Data[0].y, sdata2.Data.size(), sdata2.Offset, 2 * sizeof(float));
				ImPlot::EndPlot();
			}
			ImPlot::SetNextPlotLimitsX(0, history, ImGuiCond_Always);
			if (ImPlot::BeginPlot("##Rolling", NULL, NULL, ImVec2(-1, 150), 0, rt_axis, rt_axis)) {
				ImPlot::PlotLine("Data 1", &rdata1.Data[0].x, &rdata1.Data[0].y, rdata1.Data.size(), 0, 2 * sizeof(float));
				ImPlot::PlotLine("Data 2", &rdata2.Data[0].x, &rdata2.Data[0].y, rdata2.Data.size(), 0, 2 * sizeof(float));
				ImPlot::EndPlot();
			}
		}
		ImGui::End();
#endif //  0

		m_BuildPanel->Update(showBuildPanel);
		m_SettingsWindow->Update(showSettingsWindow, m_Scene);
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

		DrawSceneView(true);
		DrawGameView(showGameView);
		m_MaterialLibraryInterface->Draw(showMaterialLibrary);

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

		DrawHierarchy();
		DrawInspector();

		m_FileBrowser->Display();
		UpdateFileBrowser(showAnimationPanel);
		ImGui::End();

	}

	void EditorLayer::DrawInfo(HeadComponent* head)
	{
		m_TempActorName = head->Name;
		m_TempActorTag = head->Tag;

		if (ImGui::Extensions::InputRawString("Name", m_TempActorName))
		{
			if (m_Scene->GetActiveScene()->OnActorNameChanged(head->Name, m_TempActorName))
			{
				head->Name = m_TempActorName;
			}
		}
		ImGui::Extensions::InputString("Tag", head->Tag, m_TempActorTag);
		ImGui::Extensions::CheckBox("Enabled", head->IsEnabled, 130.0f, "HeadPanel");
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
			ImGui::Extensions::CheckBox("Bullet", rb->Body.m_IsBullet);

			ImGui::NewLine();

		}

		ImGui::Extensions::CheckBox("Trigger", rb->Body.m_IsTrigger);
		ImGui::Extensions::CheckBox("Awake", rb->Body.m_IsAwake);
		ImGui::Extensions::CheckBox("Allow Sleep", rb->Body.m_canSleep);
		ImGui::Extensions::CheckBox("Draw Shape", rb->ShowShape);

	}

	void EditorLayer::DrawCamera(CameraComponent* camera)
	{
		ImGui::Extensions::InputFloat("Zoom", camera->ZoomLevel);
		ImGui::Extensions::InputFloat("Near", camera->zNear);
		ImGui::Extensions::InputFloat("Far", camera->zFar);

		ImGui::NewLine();

		if(ImGui::Extensions::CheckBox("Primary", camera->isPrimaryCamera))
		{
			if (camera->isPrimaryCamera)
			{
				const size_t id = m_SelectedActor->GetID();

				SceneData& data = m_Scene->GetActiveScene()->GetSceneData();
				data.m_Registry.view<HeadComponent, CameraComponent>().each([&](HeadComponent& head, CameraComponent& camera)
				{
					if (head.ID != id)
						camera.isPrimaryCamera = false;
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
					AudioSystem::DebugPlay(clip, &AudioEngineSComponent::Get()->Engine);
				}

				ImGui::SameLine();

				if (ImGui::SmallButton("Stop"))
				{
					AudioSystem::DebugStop(clip, &AudioEngineSComponent::Get()->Engine);
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

				ImGui::Extensions::CheckBox("Default Clip", clip->m_bIsDefaultClip);

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

	void EditorLayer::DrawBehaviorComponent(std::vector<OutValue>& outValues)
	{

	}

	void EditorLayer::DrawLight2D(Light2DSourceComponent* light)
	{
		ImGui::Extensions::InputFloat2Base("Offset", light->Offset, 130.0f, "2DLightPanel");
		ImGui::Extensions::InputFloat("Intensity", light->Intensity, 130.0f, "2DLightPanel");
		ImGui::Extensions::InputFloat("Radius", light->Radius, 130.0f, "2DLightPanel");
		ImGui::Extensions::ColorInput3("Color", light->Color, 130.0f, "2DLightPanel");

		ImGui::NewLine();

		ImGui::Extensions::CheckBox("Enabled", light->IsEnabled, 130.0f, "2DLightPanel");
	}

	void EditorLayer::DrawSceneView(bool enabled)
	{
		bool snap = false;
		float snapValue = 0.5f;
		if (Input::IsKeyPressed(KeyCode::LeftShift))
		{
			snap = true;
		}
		if (Input::IsKeyPressed(KeyCode::Z))
		{
			m_GizmoOperation = ImGuizmo::OPERATION::SCALE;
		}
		if (Input::IsKeyPressed(KeyCode::Q))
		{
			m_GizmoOperation = ImGuizmo::OPERATION::ROTATE;
			snapValue = 45.0f;
		}
		if (Input::IsKeyPressed(KeyCode::E))
		{
			m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		}

		if (enabled)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
			ImGui::Begin("Scene View", &enabled);
			{

				if (ImGui::IsWindowHovered()) { isSceneViewFocused = true; }
				else { isSceneViewFocused = false; }

				m_SceneViewSize = { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };

				const auto& frameBuffer = GraphicsContext::GetSingleton()->GetFramebuffer();
				ImVec2 ViewPortSize = ImGui::GetContentRegionAvail();

				if (ViewPortSize.x != m_ViewPortSize.x || ViewPortSize.y != m_ViewPortSize.y)
				{
					m_ViewPortSize = { ViewPortSize.x, ViewPortSize.y };
					m_Camera->OnResize(static_cast<uint32_t>(m_ViewPortSize.y), static_cast<uint32_t>(m_ViewPortSize.x));
				}

#ifdef SMOLENGINE_OPENGL_IMPL
				ImGui::Image(frameBuffer->GetImGuiTextureID(), ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2(0, 1), ImVec2(1, 0));
#else
				ImGui::Image(frameBuffer->GetImGuiTextureID(), ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y });
#endif
				// Gizmos
				if (m_SelectedActor != nullptr && !m_Scene->IsInPlayMode())
				{
					auto transformComponent = m_Scene->GetActiveScene()->GetComponent<TransformComponent>(*m_SelectedActor.get());
					if (transformComponent)
					{
						switch (m_Camera->GetType())
						{
						case CameraType::Perspective:
						{
							ImGuizmo::SetOrthographic(false);
							break;
						}
						case CameraType::Ortho:
						{
							ImGuizmo::SetOrthographic(true);
							break;
						}
						default:
							break;
						}

						ImGuizmo::SetDrawlist();

						float width = (float)ImGui::GetWindowSize().x;
						float height = (float)ImGui::GetWindowSize().y;

						ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, width, height);

						glm::mat4 transform;
						CommandSystem::ComposeTransform(transformComponent->WorldPos, transformComponent->Rotation, transformComponent->Scale, false, transform);
						float snapValues[3] = { snapValue, snapValue, snapValue };

						ImGuizmo::Manipulate(glm::value_ptr(m_Camera->GetViewMatrix()), glm::value_ptr(m_Camera->GetProjection()),
							m_GizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

						if (ImGuizmo::IsUsing())
						{
							glm::vec3 tranlation, rotation, scale;
							CommandSystem::DecomposeTransform(transform, tranlation, rotation, scale);
							glm::vec3 deltaRot = rotation - transformComponent->Rotation;

							transformComponent->WorldPos = tranlation;
							transformComponent->Rotation.x += deltaRot.x;
							transformComponent->Scale = scale;
						}
					}
				}
			}
			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	void EditorLayer::DrawGameView(bool enabled)
	{
		return;
		if (enabled)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
			ImGui::Begin("Game View", &enabled);
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
			}

			ImGui::End();
			ImGui::PopStyleVar();

		}
	}

	void EditorLayer::DrawInspector()
	{
		ImGui::Begin("Inspector");
		{
			ImGui::BeginChild("InspectorChild");

			ImGui::SetWindowFontScale(0.9f);
			if (m_SelectedActor == nullptr || m_SelectionFlags != SelectionFlags::Inspector)
			{
				ImGui::Text("No Actor selected");
				ImGui::EndChild();
				ImGui::End();
				return;
			}
			else
			{
				std::stringstream ss;

				if (ImGui::Button("Add Component"))
					ImGui::OpenPopup("AddComponentPopUp");

				ImGui::SameLine();
				if (ImGui::Button("Add C++ Script"))
					ImGui::OpenPopup("AddCScriptPopUp");


				if (ImGui::BeginPopup("AddCScriptPopUp"))
				{
					ImGui::MenuItem("New Script", NULL, false, false);
					ImGui::Separator();

#if 0
					for (const auto& [key, name] : SystemRegistry::Get()->m_SystemMap)
					{
						if (ImGui::MenuItem(name.c_str()))
						{
							m_Scene->GetActiveScene().AddBehaviour(name, m_SelectedActor);
							ImGui::CloseCurrentPopup();
						}
					}
#endif // 0

					ImGui::EndPopup();
				}

				if (ImGui::BeginPopup("AddComponentPopUp"))
				{
					ImGui::MenuItem("New Component", NULL, false, false);
					ImGui::Separator();

					if (ImGui::BeginMenu("Base"))
					{
						if (ImGui::MenuItem("Mesh"))
						{
							m_Scene->GetActiveScene()->AddComponent<MeshComponent>(*m_SelectedActor.get());
							ImGui::CloseCurrentPopup();
						}

						if (ImGui::MenuItem("Texture"))
						{
							m_Scene->GetActiveScene()->AddComponent<Texture2DComponent>(*m_SelectedActor.get());
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Light"))
					{
						if (ImGui::MenuItem("Point Light 2D"))
						{
							m_Scene->GetActiveScene()->AddComponent<Light2DSourceComponent>(*m_SelectedActor.get());
							ImGui::CloseCurrentPopup();
						}

						if (ImGui::MenuItem("Point Light"))
						{
							m_Scene->GetActiveScene()->AddComponent<PointLightComponent>(*m_SelectedActor.get());
							ImGui::CloseCurrentPopup();
						}

						if (ImGui::MenuItem("Directional Light"))
						{
							m_Scene->GetActiveScene()->AddComponent<DirectionalLightComponent>(*m_SelectedActor.get());
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Physics"))
					{
						if (ImGui::MenuItem("Rigidbody 2D"))
						{
							m_Scene->GetActiveScene()->AddComponent<Body2DComponent>(*m_SelectedActor.get(), m_SelectedActor, 0);
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Common"))
					{

						if (ImGui::MenuItem("Animation 2D"))
						{
							m_Scene->GetActiveScene()->AddComponent<Animation2DComponent>(*m_SelectedActor.get());
							ImGui::CloseCurrentPopup();
						}

						if (ImGui::MenuItem("Audio Source"))
						{
							m_Scene->GetActiveScene()->AddComponent<AudioSourceComponent>(*m_SelectedActor.get());
							ImGui::CloseCurrentPopup();
						}

						if (ImGui::MenuItem("Camera"))
						{
							m_Scene->GetActiveScene()->AddComponent<CameraComponent>(*m_SelectedActor.get());
							ImGui::CloseCurrentPopup();
						}

						if (ImGui::MenuItem("Canvas"))
						{
							m_Scene->GetActiveScene()->AddComponent<CanvasComponent>(*m_SelectedActor.get());
							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					ImGui::EndPopup();
				}

				// Head
				if (ImGui::CollapsingHeader("Head"))
				{
					ImGui::NewLine();
					auto info = m_Scene->GetActiveScene()->GetComponent<HeadComponent>(*m_SelectedActor.get());
					DrawInfo(info);
				}

				// Transform 

				if (ImGui::CollapsingHeader("Tranform"))
				{
					ImGui::NewLine();
					DrawTransform(m_Scene->GetActiveScene()->GetComponent<TransformComponent>(*m_SelectedActor.get()));
				}

				for (uint32_t i = 0; i < m_SelectedActor->GetComponentsCount(); ++i)
				{
					if (IsCurrentComponent<Texture2DComponent>(i))
					{
						if (ImGui::CollapsingHeader("Texture 2D"))
						{
							ImGui::NewLine();
							auto component = m_Scene->GetActiveScene()->GetComponent<Texture2DComponent>(*m_SelectedActor.get());
							DrawTexture(component);
						}
					}

					if (IsCurrentComponent<Body2DComponent>(i))
					{
						if (ImGui::CollapsingHeader("Rigidbody 2D"))
						{
							ImGui::NewLine();
							auto component = m_Scene->GetActiveScene()->GetComponent<Body2DComponent>(*m_SelectedActor.get());
							DrawBody2D(component);
						}
					}

					if (IsCurrentComponent<Animation2DComponent>(i))
					{
						if (ImGui::CollapsingHeader("Animation 2D"))
						{
							ImGui::NewLine();
							auto component = m_Scene->GetActiveScene()->GetComponent<Animation2DComponent>(*m_SelectedActor.get());
							DrawAnimation2D(component);
						}
					}

					if (IsCurrentComponent<AudioSourceComponent>(i))
					{
						if (ImGui::CollapsingHeader("Audio Source"))
						{
							ImGui::NewLine();
							auto component = m_Scene->GetActiveScene()->GetComponent<AudioSourceComponent>(*m_SelectedActor.get());
							DrawAudioSource(component);
						}
					}

					if (IsCurrentComponent<Light2DSourceComponent>(i))
					{
						if (ImGui::CollapsingHeader("Light 2D"))
						{
							ImGui::NewLine();
							auto component = m_Scene->GetActiveScene()->GetComponent<Light2DSourceComponent>(*m_SelectedActor.get());
							DrawLight2D(component);
						}
					}

					if (IsCurrentComponent<MeshComponent>(i))
					{
						if (ImGui::CollapsingHeader("Mesh"))
						{
							ImGui::NewLine();
							auto component = m_Scene->GetActiveScene()->GetComponent<MeshComponent>(*m_SelectedActor.get());
							DrawMeshComponent(component);
						}
					}

					if (IsCurrentComponent<DirectionalLightComponent>(i))
					{
						if (ImGui::CollapsingHeader("Directional Light"))
						{
							ImGui::NewLine();
							auto component = m_Scene->GetActiveScene()->GetComponent<DirectionalLightComponent>(*m_SelectedActor.get());
							DrawDirectionalLightComponent(component);
						}
					}

					if (IsCurrentComponent<PointLightComponent>(i))
					{
						if (ImGui::CollapsingHeader("Point Light"))
						{
							ImGui::NewLine();
							auto component = m_Scene->GetActiveScene()->GetComponent<PointLightComponent>(*m_SelectedActor.get());
							DrawPointLightComponent(component);
						}
					}

					if (IsCurrentComponent<CameraComponent>(i))
					{
						if (ImGui::CollapsingHeader("Camera"))
						{
							ImGui::NewLine();
							auto component = m_Scene->GetActiveScene()->GetComponent<CameraComponent>(*m_SelectedActor.get());
							DrawCamera(component);
						}
					}

					DrawScriptComponent(i);
				}

				ImGui::Separator();
				ImGui::NewLine();
			}

			ImGui::EndChild();
		}

		ImGui::End();
	}

	void EditorLayer::DrawHierarchy()
	{
		ImGui::Begin("Hierarchy");
		{
			ImGui::SetWindowFontScale(0.9f);
			static char name[128];
			ImGui::InputTextWithHint("Search", "Name", name, IM_ARRAYSIZE(name));
			ImGui::Separator();
			std::string sceneStr = "Scene";
			SceneData& data = m_Scene->GetActiveScene()->GetSceneData();

			if (!data.m_Name.empty())
			{
				sceneStr = "Scene: " + data.m_Name;
			}

			ImGui::BeginChild("Scene");
			{
				if (ImGui::IsWindowHovered())
				{
					if (Input::IsMouseButtonPressed(MouseCode::Button1))
					{
						ImGui::OpenPopup("CreateActorPopUp");
					}

					if (Input::IsMouseButtonPressed(MouseCode::Button0))
						m_SelectedActor = nullptr;
				}

				if (ImGui::BeginPopup("CreateActorPopUp"))
				{
					ImGui::MenuItem("New Actor", NULL, false, false);
					ImGui::Separator();
					std::stringstream ss;

					if (ImGui::MenuItem("Empty Actor"))
					{
						ss << "New_EmptyActor_" << data.m_ActorPool.size();
						m_Scene->GetActiveScene()->CreateActor(ss.str());
					}

					if (ImGui::BeginMenu("Lights"))
					{
						if (ImGui::MenuItem("Point Light 2D"))
						{
							ss << "New_Light2D_" << data.m_ActorPool.size();
							m_Scene->GetActiveScene()->AddComponent<Light2DSourceComponent>
								(*m_Scene->GetActiveScene()->CreateActor(ss.str()).get());
						}

						if (ImGui::MenuItem("Point Light"))
						{
							ss << "New_PointLight_" << data.m_ActorPool.size();
							m_Scene->GetActiveScene()->AddComponent<PointLightComponent>
								(*m_Scene->GetActiveScene()->CreateActor(ss.str()).get());
						}

						if (ImGui::MenuItem("Directional Light"))
						{
							ss << "New_DirectionalLight_" << data.m_ActorPool.size();
							m_Scene->GetActiveScene()->AddComponent<DirectionalLightComponent>
								(*m_Scene->GetActiveScene()->CreateActor(ss.str()).get());
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Physics"))
					{
						if (ImGui::MenuItem("Rigidbody 2D"))
						{
							ss << "New_Rigidbody2D_" << data.m_ActorPool.size();
							Ref<Actor> actor = m_Scene->GetActiveScene()->CreateActor(ss.str());
							m_Scene->GetActiveScene()->AddComponent<Body2DComponent>(*actor,
								actor, 0);
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("2D"))
					{
						if (ImGui::MenuItem("Sprite"))
						{
							ss << "New_Sprite_" << data.m_ActorPool.size();
							m_Scene->GetActiveScene()->AddComponent<Texture2DComponent>
								(*m_Scene->GetActiveScene()->CreateActor(ss.str()).get());
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("3D"))
					{
						if (ImGui::MenuItem("Mesh"))
						{
							ss << "New_Mesh_" << data.m_ActorPool.size();
							m_Scene->GetActiveScene()->AddComponent<MeshComponent>
								(*m_Scene->GetActiveScene()->CreateActor(ss.str()).get());
						}


						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Common"))
					{
						if (ImGui::MenuItem("Audio Source"))
						{
							ss << "New_AudioSource_" << data.m_ActorPool.size();
							m_Scene->GetActiveScene()->AddComponent<AudioSourceComponent>
								(*m_Scene->GetActiveScene()->CreateActor(ss.str()).get());
						}

						if (ImGui::MenuItem("Animation 2D"))
						{
							ss << "New_Animation2D_" << data.m_ActorPool.size();
							m_Scene->GetActiveScene()->AddComponent<Animation2DComponent>
								(*m_Scene->GetActiveScene()->CreateActor(ss.str()).get());
						}

						if (ImGui::MenuItem("Canvas"))
						{
							ss << "New_Canvas_" << data.m_ActorPool.size();
							m_Scene->GetActiveScene()->AddComponent<CanvasComponent>
								(*m_Scene->GetActiveScene()->CreateActor(ss.str()).get());
						}

						if (ImGui::MenuItem("Camera"))
						{
							ss << "New_Camera_" << data.m_ActorPool.size();
							m_Scene->GetActiveScene()->AddComponent<CameraComponent>
								(*m_Scene->GetActiveScene()->CreateActor(ss.str()).get());
						}

						ImGui::EndMenu();
					}


					ImGui::EndPopup();
				}

				if (ImGui::TreeNodeEx(sceneStr.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					std::vector<Ref<Actor>> actors;
					m_Scene->GetActiveScene()->GetSortedActorList(actors);
					for (const auto& actor : actors)
					{
						auto result = actor->GetName().find(name);
						if (result == std::string::npos)
						{
							continue;
						}

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
						m_Scene->GetActiveScene()->DuplicateActor(m_SelectedActor);
					}

					if (ImGui::MenuItem("Delete"))
					{
						m_Scene->GetActiveScene()->DeleteActor(m_SelectedActor);
					}

					ImGui::EndPopup();
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}

	void EditorLayer::DrawMeshComponent(MeshComponent* meshComponent)
	{
		static bool showMeshInspector = false;
		if (meshComponent->Mesh)
		{
			ImGui::Extensions::Text("Material ID", std::to_string(meshComponent->Mesh->GetMaterialID()));
			ImGui::NewLine();

			ImGui::Extensions::CheckBox("Show", meshComponent->bShow);
			ImGui::Extensions::CheckBox("Static", meshComponent->bIsStatic);
			ImGui::Extensions::CheckBox("Cast Shadows", meshComponent->bIsStatic);

			ImGui::NewLine();

			if (!showMeshInspector)
			{
				if (ImGui::Button("Mesh Inspector", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
				{
					showMeshInspector = true;
				}
			}

			DrawMeshInspector(showMeshInspector);
		}

		if (ImGui::Button("Load Mesh", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
		{
			const auto& result = FileDialog::OpenFile("glTF (*.glb)\0*.glb\0FBX (*.fbx)\0*.fbx\0OBJ (*.obj)\0*.obj\0");
			if (result.has_value())
			{
				meshComponent->MaterialNames.clear();
				meshComponent->Mesh = nullptr;

				meshComponent->Mesh = Mesh::Create(result.value());
				meshComponent->FilePath = result.value();
			}
		}
		ImGui::NewLine();
	}

	void EditorLayer::DrawDirectionalLightComponent(DirectionalLightComponent* light)
	{
		if (ImGui::Extensions::CheckBox("Cast Shadows", light->bCastShadows))
		{
			entt::registry& registry = m_Scene->GetActiveScene()->GetSceneData().m_Registry;
			const auto& view = registry.view<DirectionalLightComponent>();
			for (const auto& entity : view)
			{
				auto& lightRef = view.get<DirectionalLightComponent>(entity);

				if (light->bCastShadows)
				{
					if (&lightRef != light)
						lightRef.bCastShadows = false;
					continue;
				}

				lightRef.bCastShadows = false;
			}
		}

		ImGui::Extensions::InputFloat("Intensity", light->Intensity);
		ImGui::Extensions::DragFloat3Base("Direction", light->Direction);
		ImGui::Extensions::ColorInput3("Color", light->Color);
	}

	void EditorLayer::DrawMeshInspector(bool& show)
	{
		auto* meshComponent = m_Scene->GetActiveScene()->GetComponent<MeshComponent>(*m_SelectedActor.get());
		if (!meshComponent)
			return;

		std::vector<Mesh*> meshes(meshComponent->Mesh->GetSubMeshes().size() + 1);
		uint32_t index = 0;
		meshes[index] = meshComponent->Mesh.get();
		index++;

		for (auto& sub : meshComponent->Mesh->GetSubMeshes())
		{
			meshes[index] = sub.get();
			index++;
		}

		if (show)
		{
			ImGui::Begin("Mesh Inspector", &show);
			{
				ImGui::Extensions::Text("Mesh & SubMeshes", "");
				for (auto& mesh : meshes)
				{
					std::string name = "Mesh #" + mesh->GetName();
					if (ImGui::CollapsingHeader(name.c_str()))
					{
						std::string id = name + "IDMat";
						ImGui::PushID(id.c_str());
						{
							const auto& matName = MaterialLibrary::GetSinglenton()->GetMaterialName(mesh->GetMaterialID());
							if (matName.has_value())
								ImGui::Extensions::Text("Material Name", matName.value());

							ImGui::Extensions::Text("Material ID", std::to_string(mesh->GetMaterialID()));

							if (ImGui::Button("Select Material"))
							{
								MaterialCreateInfo materialCI = {};
								const auto& result = FileDialog::OpenFile("");
								if (result.has_value())
								{
									std::string value = result.value();
									MaterialLibrary::GetSinglenton()->Load(value, materialCI);
									if (!CommandSystem::SetMeshMaterial(meshComponent, mesh, &materialCI, value))
									{
										CONSOLE_ERROR("Could not set material!");
									}
								}
							}
						}
						ImGui::PopID();
					}
				}
			}
			ImGui::End();
		}
	}

	void EditorLayer::DrawPointLightComponent(PointLightComponent* light)
	{
		ImGui::Extensions::CheckBox("Enabled", light->bEnabled);
		ImGui::Extensions::InputFloat("Constant", light->Constant);
		ImGui::Extensions::InputFloat("Linear", light->Linear);
		ImGui::Extensions::InputFloat("Exposure", light->Exposure);
		ImGui::Extensions::InputFloat3Base("Offset", light->Offset);
		ImGui::Extensions::ColorInput3("Color", light->Color);
	}

	void EditorLayer::UpdateFileBrowser(bool& showAnimPanel)
	{
		if (m_FileBrowser->HasSelected())
		{

			switch (m_FileBrowserState)
			{
			case FileBrowserFlags::Load_Jinx_Script:

				ResetFileBrowser();
				break;

			case FileBrowserFlags::Load_Texture2D:
			{
				auto texture2D = m_Scene->GetActiveScene()->GetComponent<Texture2DComponent>(*m_SelectedActor.get());
				m_Scene->GetActiveScene()->DeleteAsset(texture2D->FileName);

				texture2D->FileName = m_FileName;
				texture2D->Texture = Texture::Create(m_FilePath);

				m_Scene->GetActiveScene()->AddAsset(m_FileName, m_FilePath);
				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Load_Animation_Clip:
			{
				m_AnimationPanel->Load(m_FilePath);
				ResetFileBrowser();
				showAnimPanel = true;

				break;
			}
			case FileBrowserFlags::Load_Audio_Clip:
			{
				auto clip = std::make_shared<AudioClip>();
				auto audio = m_Scene->GetActiveScene()->GetComponent<AudioSourceComponent>(*m_SelectedActor.get());

				if (audio != nullptr)
				{
					std::stringstream ss;
					ss << "New Audio Clip #" << audio->AudioClips.size();

					clip->FileName = m_FileName;
					clip->FilePath = m_FilePath;
					clip->ClipName = ss.str();

					if (AudioSystem::AddClip(*audio, clip))
					{
						m_Scene->GetActiveScene()->AddAsset(m_FileName, m_FilePath);
					}
				}

				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Canvas_Chanage_Button_Texture:
			{
				auto canvas = m_Scene->GetActiveScene()->GetComponent<CanvasComponent>(*m_SelectedActor.get());
				if (canvas != nullptr)
				{
					auto button = UISystem::GetButton(*canvas, m_IDBuffer);
					if (button != nullptr)
					{
						m_Scene->GetActiveScene()->DeleteAsset(button->m_TetxureName);

						m_Scene->GetActiveScene()->AddAsset(m_FileName, m_FilePath);
						button->Init(m_FilePath, m_FileName);
					}
				}

				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Canavas_Create_TextLabel:
			{
				auto canvas = m_Scene->GetActiveScene()->GetComponent<CanvasComponent>(*m_SelectedActor.get());
				if (canvas != nullptr)
				{
					auto element = UISystem::AddElement(*canvas, UIElementType::TextLabel);
					auto textLabel = std::static_pointer_cast<UITextLabel>(element);

					m_Scene->GetActiveScene()->AddAsset(m_FileName, m_FilePath);
					textLabel->Init(m_FilePath, m_FileName);
				}

				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Canavas_TextLabel_Load_Font:
			{
				auto canvas = m_Scene->GetActiveScene()->GetComponent<CanvasComponent>(*m_SelectedActor.get());
				if (canvas != nullptr)
				{
					auto text = UISystem::GetTextLabel(*canvas, m_IDBuffer);
					if (text != nullptr)
					{
						m_Scene->GetActiveScene()->DeleteAsset(text->m_FontName);

						text->m_FontFilePath = m_FilePath;
						text->m_FontName = m_FileName;
						text->SetFont(m_FilePath);

						m_Scene->GetActiveScene()->AddAsset(m_FileName, m_FilePath);
					}
				}

				ResetFileBrowser();
				break;
			}
			case FileBrowserFlags::Load_Animation_Clip_Inspector:
			{
				auto animation2d = m_Scene->GetActiveScene()->GetComponent<Animation2DComponent>(*m_SelectedActor.get());
				if (animation2d)
				{
					if (Animation2DSystem::LoadClip(*animation2d, m_FilePath))
					{
						m_Scene->GetActiveScene()->AddAsset(m_FileName, m_FilePath);
					}
				}

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
		m_FileName = "";
		m_IDBuffer = 0;
		m_FileBrowserState = FileBrowserFlags::None;
	}

	void EditorLayer::DrawScriptComponent(uint32_t index)
	{
		if (m_Scene->GetActiveScene()->HasComponent<BehaviourComponent>(*m_SelectedActor.get()))
		{
			BehaviourComponent* comp = m_Scene->GetActiveScene()->GetComponent<BehaviourComponent>(*m_SelectedActor.get());
			std::string scriptName = "";
			BehaviourComponent::OutData* data = nullptr;
			for (auto& [name, container] : comp->OutValues)
			{
				if (container.ScriptID == index)
				{
					data = &container;
					scriptName = name;
					break;
				}
			}

			if (!data)
				return;

			if (ImGui::CollapsingHeader(scriptName.c_str()))
			{
				ImGui::NewLine();
				ImGui::Extensions::Text("Script Type", "C++ Script");
				ImGui::NewLine();

				for (auto& f : data->Floats)
					ImGui::Extensions::InputFloat(f.Name, f.Value);

				for (auto& i : data->Ints)
					ImGui::Extensions::InputInt(i.Name, i.Value);

				for (auto& str : data->Strings)
					ImGui::Extensions::InputRawString(str.Name, str.Value, "Value");

				ImGui::NewLine();
			}
		}
	}
}