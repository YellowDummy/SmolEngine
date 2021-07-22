#pragma once

#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"
#include "Scripting/FieldManager.h"

#include <string>
#include <vector>

#include <meta/meta.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	class Actor;
	struct ScriptComponent: public BaseComponent
	{
		ScriptComponent() = default;
		ScriptComponent(uint32_t id)
			:BaseComponent(id) {}

		struct CPPInstance
		{										   
			meta::any             Instance;
			std::string           Name = "";
			FieldManager          Fields;

			template<typename Archive>
			void serialize(Archive& archive)
			{
				archive(Name, Fields);
			}
		};	

		struct CSharpInstance
		{
			void*                Instance = nullptr;
			std::string          Name = "";
			FieldManager         Fields;

			template<typename Archive>
			void serialize(Archive& archive)
			{
				archive(Name, Fields);
			}
		};

		Ref<Actor>                   pActor = nullptr;
		std::vector<CPPInstance>     CppScripts;
		std::vector<CSharpInstance>  CSharpScripts;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(CppScripts, CSharpScripts, pActor, ComponentID);
		}
	};
}