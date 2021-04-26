#pragma once
#include "Core/Core.h"

#include <meta/meta.hpp>
#include <string>

namespace SmolEngine
{
	struct ScriptingSystemStateSComponent
	{
		ScriptingSystemStateSComponent();
		~ScriptingSystemStateSComponent();

		// Getters
		static ScriptingSystemStateSComponent* GetSingleton();

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

		std::hash<std::string_view>                    Hash{};
		std::unordered_map<std::string, MetaData>      MetaMap;

	private:
		inline static ScriptingSystemStateSComponent* Instance = nullptr;
	};
}
