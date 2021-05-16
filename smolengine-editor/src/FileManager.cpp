#include "stdafx.h"
#include "FileManager.h"
#include "FileBrowser/ImFileDialog.h"

#include <Frostium3D/Vulkan/VulkanTexture.h>

namespace SmolEngine
{
	void FileManager::Init()
	{
		ifd::FileDialog& insatnce = ifd::FileDialog::Instance();

		insatnce.CreateTexture = [&](uint8_t* data, int w, int h, char fmt) -> void*
		{
			void* descriptor = nullptr;
			VkFormat format = (fmt == 0) ? VK_FORMAT_B8G8R8A8_UNORM : VK_FORMAT_R8G8B8A8_UNORM;
			const uint32_t width = static_cast<uint32_t>(w);
			const uint32_t height = static_cast<uint32_t>(h);
			const uint32_t mipLevels = static_cast<uint32_t>(floor(log2(std::max(width, height)))) + 1;

			Frostium::VulkanTexture* tex = new Frostium::VulkanTexture();
			tex->CreateTexture(width, height, mipLevels, data, format, true);
			descriptor = tex->GetImGuiTextureID();

			m_FileTextures[descriptor] = tex;
			return descriptor;
		};

		insatnce.DeleteTexture = [&](void* tex)
		{
			const auto& it = m_FileTextures.find(tex);
			if (it != m_FileTextures.end())
			{
				Frostium::VulkanTexture* tex = it->second;
				delete tex;
			}
		};

		insatnce.SetZoom(4);
		insatnce.Open("ContentBrowser", "Content Browser", "files (*.png;*.jpg;*.jpeg;*.gltf;*.s_scene;*.s_material){.png,.jpg,.jpeg,.gltf,.s_scene,.s_material},.*");
	}

	void FileManager::Open()
	{
		ifd::FileDialog::Instance().Open();
	}

	void FileManager::Update()
	{
		ifd::FileDialog::Instance().IsDone("ContentBrowser");
	}
}