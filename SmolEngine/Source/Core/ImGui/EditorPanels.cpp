#include "stdafx.h"
#include "EditorPanels.h"

#include "Core/ECS/WorldAdmin.h"
#include "Core/Application.h"
#include "Core/SLog.h"

#include "Core/ImGui/EditorConsole.h"
#include "Core/ImGui/FileBrowser/imfilebrowser.h"
#include "Core/ImGui/ImGuiExtension.h"

#include "Core/Animation/AnimationClip.h"
#include "Core/Renderer/Renderer2D.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <box2d/box2d.h>


namespace SmolEngine
{
	void SettingsWindow::Update(bool& isOpened, Ref<WorldAdmin> scene)
	{
		if (isOpened)
		{
			ImGui::Begin("Settings", &isOpened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);
			//ImGui::SetWindowPos(ImVec2{ (float)Application::GetApplication().GetWindowWidth() / 2 - 350, 
//(float)Application::GetApplication().GetWindowHeight() / 2 - 180 });
			ImGui::SetWindowSize("Settings", { 480, 380 });

			static char name[128] = "Default Name";
			ImGui::NewLine();
			ImGui::InputText("Scene Name", name, IM_ARRAYSIZE(name));
			ImGui::PushItemWidth(200);
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputFloat2("Gravity", glm::value_ptr(scene->GetSceneData().m_Gravity));
			ImGui::PushItemWidth(200);
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputFloat("Ambient Light Strength", &scene->GetSceneData().m_AmbientStrength);
			ImGui::PushItemWidth(200);
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::Button("Update"))
			{
				auto& data = scene->GetSceneData();
				scene->GetSceneData().m_Name = name;

				isOpened = false;
			}

			ImGui::End();
		}
	}

	void ActorCreationWindow::Update(bool& isOpened, Ref<WorldAdmin> scene)
	{
		if (isOpened)
		{
			ImGui::Begin("Actor Factory", &isOpened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);
			ImGui::SetWindowSize("Actor Factory", { 480, 220 });

			static std::string name = "";
			static std::string tag = "";
			static int comboValue = 0;

			ImGui::BeginChild("Actor Data", { 480, 140 });
			ImGui::NewLine();

			ImGui::PushID("ActorFactoryName");
			ImGui::Extensions::InputRawString("Name", name, "Name", 130.0f, false);
			ImGui::PopID();

			ImGui::PushID("ActorFactoryTag");
			ImGui::Extensions::InputRawString("Tag", tag, "Tag", 130.0f, false);
			ImGui::PopID();

			ImGui::Extensions::Combo("Base Type", "Default Actor\0Physics Actor\0Camera Actor\0\0", comboValue);

			ImGui::NewLine();
			ImGui::EndChild();

			if (ImGui::Button("Create", { ImGui::GetWindowWidth() - 10.0f, 30.0f }))
			{
				if (name.empty())
				{
					CONSOLE_WARN("Actor was not created, name is empty");
				}
				else
				{
					if (tag.empty())
					{
						tag = "Default";
					}

					Ref<WorldAdmin> scene = WorldAdmin::GetScene();
					Ref<Actor> actor = nullptr;

					// Default

					if (comboValue == 0)
					{
						actor = scene->CreateActor(ActorBaseType::DefaultBase, name, tag);
					}

					// Camera

					if (comboValue == 2)
					{
						actor = scene->CreateActor(ActorBaseType::CameraBase, name, tag);
					}

					if (actor == nullptr)
					{
						CONSOLE_ERROR("Actor was not created, it already exists");
					}

					name = "";
					tag = "";
					comboValue = 0;

					isOpened = false;
				}

			}

			ImGui::End();
		}
	}
}