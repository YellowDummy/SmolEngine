#pragma once
#include "Core/Core.h"

#include <../Libraries/meta/meta.hpp>
#include <string>

namespace SmolEngine
{
	struct ScriptingSystemStateSComponent
	{
		ScriptingSystemStateSComponent();

		~ScriptingSystemStateSComponent();

		// Data

		struct MetaData
		{
			meta::any  ClassInstance;

			meta::func OnBeginFunc;
			meta::func OnProcessFunc;
			meta::func OnDestroyFunc;

			meta::func OnCollBeginFunc;
			meta::func OnCollEndFunc;
			meta::func OnDebugDrawFunc;
		};

		inline static ScriptingSystemStateSComponent*  Instance = nullptr;
		std::hash<std::string_view>                    Hash{};
		std::unordered_map<std::string, MetaData>      MetaMap;

		// Getters
		static ScriptingSystemStateSComponent* GetSingleton();
	};
}
