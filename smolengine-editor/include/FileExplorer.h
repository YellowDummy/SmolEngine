#pragma once
#include "Core/Core.h"
#include "TexturesLoader.h"

#include <string>
#include <vector>
#include <future>
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
		void Update();

	private:

		struct Directory
		{
			std::unordered_map<std::string, Ref<Texture>> m_IconsMap = {};
		};

		void DrawHierarchy();
		void DrawDirectory(const std::filesystem::path& path);
		void DrawNode(const std::filesystem::path& path, Directory& owner);
		void DrawIcon(Texture* icon, bool flip = false);
		void DrawPopUp();
	private:

		int                                           m_SelectionIndex = 0;
		TexturesLoader*                               m_TextureLoader = nullptr;
		glm::vec2                                     m_ButtonSize = glm::vec2(20.0f);
		std::string                                   m_DirPopUpPath;
		std::string                                   m_NodePopUpPath;
		std::string                                   m_CurrentDir;
		std::string                                   m_SelectedNode;
		std::unordered_map<std::string, Directory>    m_OpenDirectories = {};
		std::vector<const char*>                      m_FileExtensions = { ".png", ".jpg", ".s_scene", ".s_material", ".gltf" };
	};
}