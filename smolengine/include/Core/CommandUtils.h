#pragma once

#include <string>

namespace SmolEngine
{
	class CommandUtils
	{
	public:

		static void UpdatePath(std::string& path, const std::string& searchDir);
		static uint32_t GetNumFilenameDublicates(const std::string& fileName, const std::string& directory);
	};
}