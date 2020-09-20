#include "stdafx.h"
#include "EditorPanels.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <box2d/box2d.h>

#include "Core/ECS/Scene.h"
#include "Core/Application.h"
#include "Core/SLog.h"
#include "Core/ImGui/EditorConsole.h"
#include "Core/ImGui/FileBrowser/imfilebrowser.h"
#include "Core/Animation/Animation2D.h"
#include "Core/Renderer/Renderer2D.h"

namespace SmolEngine
{
	void SettingsWindow::Update(bool& isOpened, Ref<Scene> scene)
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
				scene->m_World = new b2World({ data.m_Gravity.x, data.m_Gravity.y });

				isOpened = false;
			}

			ImGui::End();
		}
	}

	void ActorCreationWindow::Update(bool& isOpened, Ref<Scene> scene)
	{
		if (isOpened)
		{
			ImGui::Begin("Create Actor", &isOpened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);
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
				auto ref = scene->CreateActor(name, tag);
				if (!ref)
				{
					EditorConsole::GetConsole()->AddMessage(std::string("Actor was not created, it already exists"), LogLevel::Error);
				}

				isOpened = false;
			}
			ImGui::End();
		}
	}
}