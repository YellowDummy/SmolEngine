#include "stdafx.h"
#include "EditorPanels.h"

#include "ECS/WorldAdmin.h"
#include "Core/Application.h"
#include "Core/SLog.h"

#include "ImGui/EditorConsole.h"
#include "ImGui/FileBrowser/imfilebrowser.h"
#include "ImGui/ImGuiExtension.h"

#include "Animation/AnimationClip2D.h"
#include "Renderer/Renderer2D.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <box2d/box2d.h>


namespace SmolEngine
{
	void SettingsWindow::Update(bool& isOpened, WorldAdmin* scene)
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

			ImGui::InputFloat2("Gravity", glm::value_ptr(scene->GetActiveScene().GetSceneData().m_Gravity));
			ImGui::PushItemWidth(200);
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputFloat("Ambient Light Strength", &scene->GetActiveScene().GetSceneData().m_AmbientStrength);
			ImGui::PushItemWidth(200);
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::Button("Update"))
			{
				scene->GetActiveScene().GetSceneData().m_Name = name;
				isOpened = false;
			}

			ImGui::End();
		}
	}
}