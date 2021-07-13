#pragma once
#include <Core/Core.h>

#include <Frostium3D/Primitives/Texture.h>

namespace SmolEngine
{
	struct TexturesLoader
	{
		TexturesLoader()
		{
			TextureCreateInfo texCI = {};
			texCI.bImGUIHandle = true;

			texCI.FilePath = "assets/buttons/play_button.png";
			Texture::Create(&texCI, &m_PlayButton);

			texCI.FilePath = "assets/buttons/pause_button.png";
			Texture::Create(&texCI, &m_StopButton);

			texCI.FilePath = "assets/buttons/move_button.png";
			Texture::Create(&texCI, &m_MoveButton);

			texCI.FilePath = "assets/buttons/rotate_button.png";
			Texture::Create(&texCI, &m_RotateButton);

			texCI.FilePath = "assets/buttons/scale_button.png";
			Texture::Create(&texCI, &m_ScaleButton);

			texCI.FilePath = "assets/buttons/search_button.png";
			Texture::Create(&texCI, &m_SearchButton);

			texCI.FilePath = "assets/buttons/remove_button.png";
			Texture::Create(&texCI, &m_RemoveButton);

			texCI.FilePath = "assets/buttons/folder_button.png";
			Texture::Create(&texCI, &m_FolderButton);

			texCI.FilePath = "assets/buttons/documents_button.png";
			Texture::Create(&texCI, &m_DocumentsIcon);

			texCI.FilePath = "assets/buttons/scene_button.png";
			Texture::Create(&texCI, &m_SceneIcon);

			texCI.FilePath = "assets/buttons/material_button.png";
			Texture::Create(&texCI, &m_MaterialIcon);

			texCI.FilePath = "assets/buttons/default_background.jpg";
			Texture::Create(&texCI, &m_BackgroundIcon);

			texCI.FilePath = "assets/buttons/cube_icon.png";
			Texture::Create(&texCI, &m_CubeIcon);

			texCI.FilePath = "assets/buttons/sphere_icon.png";
			Texture::Create(&texCI, &m_SphereIcon);

			texCI.FilePath = "assets/buttons/capsule_icon.png";
			Texture::Create(&texCI, &m_CapsuleIcon);

			texCI.FilePath = "assets/buttons/torus_icon.png";
			Texture::Create(&texCI, &m_TorusIcon);

			texCI.FilePath = "assets/buttons/return_button.png";
			Texture::Create(&texCI, &m_ReturnIcon);

			texCI.FilePath = "assets/buttons/forward_button.png";
			Texture::Create(&texCI, &m_ForwardIcon);

			texCI.FilePath = "assets/buttons/update_button.png";
			Texture::Create(&texCI, &m_UpdateIcon);

			texCI.FilePath = "assets/buttons/glTF_button.png";
			texCI.bVerticalFlip = false;
			Texture::Create(&texCI, &m_glTFIcon);

			s_Instance = this;
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
		Texture  m_CubeIcon{};
		Texture  m_SphereIcon{};
		Texture  m_CapsuleIcon{};
		Texture  m_TorusIcon{};
		Texture  m_ReturnIcon{};
		Texture  m_ForwardIcon{};
		Texture  m_UpdateIcon{};
	private:

		inline static TexturesLoader* s_Instance = nullptr;
	};
}