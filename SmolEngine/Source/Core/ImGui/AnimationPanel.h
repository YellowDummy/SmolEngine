#pragma once

#include "Core/Core.h"
#include <../../imgui/imgui.h>
#include "Core/ImGui/FileBrowser/imfilebrowser.h"

namespace SmolEngine
{
	class Animation2D;
	class CameraController;
	struct Animation2DFrameKey;

	enum class AnimationPanelSelectionState : uint16_t
	{
		None = 0,
		NewFrame, NewSprite, Save, Load
	};

	class AnimationPanel
	{
	public:

		AnimationPanel();

		void Update(bool& isOpened);

	private:

		void Save(const std::string& filePath);

		void Load(const std::string& filePath);


	private:

		friend class EditorLayer;

		std::unique_ptr<CameraController> m_Camera = nullptr;
		std::unique_ptr<Animation2D> m_AnimationClip = nullptr;
		std::unique_ptr<ImGui::FileBrowser> m_FileBrowser = nullptr;

		std::shared_ptr<Animation2DFrameKey> m_SelectedFrame = nullptr;

		AnimationPanelSelectionState m_SelectionState = AnimationPanelSelectionState::None;

	};
}
