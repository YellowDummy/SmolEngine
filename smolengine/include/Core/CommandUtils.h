#pragma once

#include <string>

namespace SmolEngine
{
	class CommandUtils
	{
	public:

		static void UpdatePath(std::string& path, const std::string& searchDir);
	};
}