#include "stdafx.h"
#include "Core/CommandUtils.h"

#include <Frostium3D/MaterialLibrary.h>

namespace SmolEngine
{
	void CommandUtils::UpdatePath(std::string& path, const std::string& searchDir)
	{
		std::filesystem::path origPath(path);
		std::string origName = origPath.filename().u8string();

		for (auto& p : std::filesystem::recursive_directory_iterator(searchDir))
		{
			std::string name = p.path().filename().u8string();
			if (name == origName)
			{
				path = p.path().u8string();
				break;
			}
		}
	}

	uint32_t CommandUtils::GetNumFilenameDublicates(const std::string& fileName, const std::string& directory)
	{
		uint32_t count = 0;
		for (auto& p : std::filesystem::directory_iterator(directory))
		{
			std::string it_fileName = p.path().filename().u8string();
			if (it_fileName.find(fileName) != std::string::npos)
			{
				count++;
			}
		}

		return count;
	}
}
