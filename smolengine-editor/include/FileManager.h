#pragma once

#include <unordered_map>

class Frostium::VulkanTexture;

namespace SmolEngine
{
	class FileManager
	{
	public:
		
		void Init();
		void Update(bool& enbaled);

	private:

		std::unordered_map<void*, Frostium::VulkanTexture*>  m_FileTextures;
	};
}