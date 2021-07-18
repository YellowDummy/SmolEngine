#pragma once
#include "Core/Core.h"

#include <meta/meta.hpp>
#include <string>

namespace SmolEngine
{
	class MetaContext;
	class MonoContext;

	struct ScriptingSystemStateSComponent
	{
		ScriptingSystemStateSComponent();
		~ScriptingSystemStateSComponent();
		static ScriptingSystemStateSComponent* GetSingleton();

		MetaContext* m_MetaContext = nullptr;
		MonoContext* m_MonoContext = nullptr;

	private:
		inline static ScriptingSystemStateSComponent* Instance = nullptr;
	};
}
