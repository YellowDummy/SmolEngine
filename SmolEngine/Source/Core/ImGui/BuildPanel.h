#pragma once

#include "Core/BuildConfig.h"

#include <../../Libraries/imgui/imgui.h>
#include "Core/ImGui/FileBrowser/imfilebrowser.h"

namespace SmolEngine
{
	enum class BuildPanelFileBrowserState: uint16_t
	{
		None = 0,
		Add_Scene, Save_Config, Load_Config
	};

	class BuildPanel
	{
	public:

		BuildPanel();

		void Update(bool& isOpen);

		void Clear();

	private:

		void ResetFileBrowser();

		bool Save(const std::string& filePath);

		bool Load(const std::string& filePath);

	private:

		friend class EditorLayer;

		BuildConfig m_CurrentBuildConfig;
		std::unique_ptr<ImGui::FileBrowser> m_FileBrowser = nullptr;

		float m_xOffset = 300.0f;


		BuildPanelFileBrowserState m_FileBrowserState = BuildPanelFileBrowserState::None;
	};
}