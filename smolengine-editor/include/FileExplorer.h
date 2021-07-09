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

		void Create(const std::string& current_path);
		void ClearSelection();
		void Update();
		void Import();

		void SetOnFileSelectedCallback(const std::function<void(const std::string&, const std::string&, int)>& callback);
		void SetOnFileDeletedCallaback(const std::function<void(const std::string&, const std::string&)>& callback);

	private:
		enum class PendeingActionFlags
		{
			None    = 0,
			Rename  = 1,
			NewFile = 2,
		};

		enum class PopUpFlags
		{
			None    = 0,
			Node    = 1,
			Folder  = 2,
		};

		struct PendeingAction
		{
			PendeingActionFlags Type;
			std::string         Path;
			std::string         NewName;
		};

		void DrawHierarchy();
		void DrawDirectory(const std::filesystem::path& path);
		void DrawNode(const std::filesystem::path& path);
		void DrawIcon(Texture* icon, bool flip = false);
		void DrawIcon(const std::string& ext = "");
		void DrawNodeIcon(const std::string& path, const std::string& ext, Ref<Texture>& icon);
		void DrawPopUp();
		void AddPendingAction(const std::string& path, PendeingActionFlags action);
		void ClosePopUp();
		void Reset();
	private:

		std::function<void(const std::string&, const std::string&, int)>  m_pOnFileSelected = nullptr;
		std::function<void(const std::string&, const std::string&)>       m_pOnFileDeleted = nullptr;
		TexturesLoader*                                                   m_pTextureLoader = nullptr;
		PendeingAction*                                                   m_pPendeingAction = nullptr;
		PopUpFlags                                                        m_ePopUpFlags = PopUpFlags::None;
		int                                                               m_SelectionIndex = 0;
		glm::vec2                                                         m_ButtonSize = glm::vec2(23.0f);
		ImVec4                                                            m_SelectColor = ImVec4(0.984f, 0.952f, 0.356f, 1.0f);
		std::string                                                       m_PopUpBuffer;
		std::string                                                       m_CurrentDir;
		std::string                                                       m_HomeDir;
		std::string                                                       m_SelectedNode;
		std::string                                                       m_SearchBuffer;
		std::string                                                       m_DragAndDropBuffer;
		std::unordered_map<std::string, Ref<Texture>>                     m_IconsMap = {};
		std::vector<const char*>                                          m_FileExtensions = { ".s_image", ".s_scene", ".s_material", ".gltf" };
	};													                  
}