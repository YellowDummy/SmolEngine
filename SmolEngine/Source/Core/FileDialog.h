#pragma once
#include "Core/Core.h"

#include <optional>

namespace SmolEngine
{
	class FileDialog
	{
	public:

		static std::optional<std::string> OpenFile(const char* filter);

		static std::optional<std::string> SaveFile(const char* filter, const std::string& initialName = "");
	};
}