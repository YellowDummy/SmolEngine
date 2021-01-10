#include "stdafx.h"
#include "EditorConsole.h"

namespace SmolEngine
{
	std::shared_ptr<EditorConsole> EditorConsole::s_EditorConsole = std::make_shared<EditorConsole>();
}