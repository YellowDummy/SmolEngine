#pragma once
#include "Core/Core.h"
#include "TexturesLoader.h"

#include <string>
#include <vector>
#include <future>
#include <functional>
#include <unordered_map>
#include <glm/glm/glm.hpp>

namespace SmolEngine
{
	class EditorLayer;
	class Texture;

	class FileExplorer
	{
	public:

		void Create(const std::string& current_path, TexturesLoader* loader);
		void SetNodeSelectedCallback(std::function<void(const std::string&, const std::string&)>& callback);
		void Update();

	private:
		enum class PendeingActionFlags
		{
			None   = 0,
			Rename = 1,
			Delete = 2,
		};

		enum class PopUpFlags
		{
			None   = 0,
			Node   = 1,
			Folder = 2,
		};

		struct PendeingAction
		{
			PendeingActionFlags Type;
			std::string         Path;
			std::string         NewName;
		};

		struct Directory
		{
			std::unordered_map<std::string, Ref<Texture>> m_IconsMap = {};
		};

		void DrawHierarchy();
		void DrawDirectory(const std::filesystem::path& path);
		void DrawNode(const std::filesystem::path& path, Directory& owner);
		void DrawIcon(Texture* icon, bool flip = false);
		void DrawPopUp();
		void ClosePopUp();
		void Reset();
	private:

		std::function<void(const std::string&,
			const std::string&)>                         m_pOnNodeSelected = nullptr;
		TexturesLoader*                                  m_pTextureLoader = nullptr;
		PendeingAction*                                  m_pPendeingAction = nullptr;
		PopUpFlags                                       m_ePopUpFlags = PopUpFlags::None;
		int                                              m_SelectionIndex = 0;
		glm::vec2                                        m_ButtonSize = glm::vec2(20.0f);
		std::string                                      m_PopUpBuffer;
		std::string                                      m_CurrentDir;
		std::string                                      m_SelectedNode;
		std::string                                      m_DragAndDropBuffer;
		std::unordered_map<std::string, Directory>       m_OpenDirectories;
		std::vector<const char*>                         m_FileExtensions = { ".png", ".jpg", ".s_scene", ".s_material", ".gltf" };
	};
}