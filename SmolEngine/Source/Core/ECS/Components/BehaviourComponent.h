#pragma once

#include "Core/Core.h"
#include "Core/ECS/Components/BaseComponent.h"
#include "Core/Scripting/SystemRegistry.h"
#include "Core/Scripting/OutValues.h"

#include <string>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	class Actor;

	struct BehaviourComponent: public BaseComponent
	{
		BehaviourComponent();

		BehaviourComponent(uint32_t id) 
			:BaseComponent(id) {}

		/// Data

		struct OutData
		{
			std::vector<OutValue> OutValues;
			int32_t ScriptID = 0;

			template<typename Archive>
			void serialize(Archive& archive)
			{
				archive(ScriptID, OutValues);
			}
		};

		std::vector<ScriptInstance> Scripts;
		std::unordered_map<std::string, OutData> OutValues;

		size_t ID = 0;
		Ref<Actor> Actor = nullptr;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(ID, OutValues, ComponentID, Actor);
		}
	};
}