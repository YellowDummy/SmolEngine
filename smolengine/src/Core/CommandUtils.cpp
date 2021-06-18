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
}
