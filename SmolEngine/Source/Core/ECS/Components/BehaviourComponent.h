#pragma once

#include "Core/Core.h"
#include "Core/ECS/Components/BaseComponent.h"
#include "Core/Scripting/OutValues.h"

#include <string>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace SmolEngine
{
	class Actor;

	struct BehaviourComponent: public BaseComponent
	{
		BehaviourComponent();

		BehaviourComponent(uint32_t id) 
			:BaseComponent(id) {}

		/// Data

		size_t ID = 0;
		
		std::vector<OutValue> OutValues;
		std::string SystemName = "";
		Ref<Actor> Actor = nullptr;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(ID, SystemName, OutValues, ComponentID);
		}
	};
}