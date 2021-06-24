#pragma once
#include <Core/Core.h>
#include <Frostium3D/Common/Texture.h>

namespace SmolEngine
{
	struct TexturesLoader
	{
		TexturesLoader()
		{
			const bool flip = true;
			const bool imguiDescriptor = true;
			s_Instance = this;

			Texture::Create("assets/buttons/play_button.png", &m_PlayButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
			Texture::Create("assets/buttons/pause_button.png", &m_StopButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
			Texture::Create("assets/buttons/move_button.png", &m_MoveButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
			Texture::Create("assets/buttons/rotate_button.png", &m_RotateButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
			Texture::Create("assets/buttons/scale_button.png", &m_ScaleButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
			Texture::Create("assets/buttons/search_button.png", &m_SearchButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
			Texture::Create("assets/buttons/remove_button.png", &m_RemoveButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
			Texture::Create("assets/buttons/folder_button.png", &m_FolderButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
			Texture::Create("assets/buttons/documents_button.png", &m_DocumentsIcon, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
			Texture::Create("assets/buttons/scene_button.png", &m_SceneIcon, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
			Texture::Create("assets/buttons/material_button.png", &m_MaterialIcon, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
			Texture::Create("assets/buttons/glTF_button.png", &m_glTFIcon, TextureFormat::R8G8B8A8_UNORM, false, imguiDescriptor);
			Texture::Create("assets/buttons/default_background.jpg", &m_BackgroundIcon, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
		}

		static TexturesLoader* Get() { return s_Instance; }

		Texture  m_PlayButton{};
		Texture  m_StopButton{};
		Texture  m_MoveButton{};
		Texture  m_ScaleButton{};
		Texture  m_RotateButton{};
		Texture  m_SearchButton{};
		Texture  m_RemoveButton{};
		Texture  m_FolderButton{};
		Texture  m_DocumentsIcon{};
		Texture  m_SceneIcon{};
		Texture  m_MaterialIcon{};
		Texture  m_glTFIcon{};
		Texture  m_BackgroundIcon{};
	private:

		inline static TexturesLoader* s_Instance = nullptr;
	};
}