#pragma once

#include <unordered_map>
#include <functional>

namespace SmolEngine
{
	class VulkanTexture;

	class FileManager
	{
	public:
		
		void Init();
		void Open();
		void Update();
		void SetMaterialCreateCallback(const std::function<void(const std::string&, bool)>& func);
		void SetFileDeleteCallback(const std::function<void(const std::string&)>& func);
	private:

		std::unordered_map<void*, VulkanTexture*>  m_FileTextures;
	};
}