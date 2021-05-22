#pragma once

#include <unordered_map>

namespace SmolEngine
{
	class VulkanTexture;

	class FileManager
	{
	public:
		
		void Init();
		void Open();
		void Update();

	private:

		std::unordered_map<void*, VulkanTexture*>  m_FileTextures;
	};
}