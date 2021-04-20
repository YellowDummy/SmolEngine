#include "stdafx.h"
#include "Core/FileDialog.h"

#include <Frostium3D/Utils/Utils.h>

namespace SmolEngine
{
	std::optional<std::string> FileDialog::OpenFile(const char* filter)
	{
		return Frostium::Utils::OpenFile(filter);
	}

	std::optional<std::string> FileDialog::SaveFile(const char* filter, const std::string& initialName)
	{
		return Frostium::Utils::SaveFile(filter);
	}
}