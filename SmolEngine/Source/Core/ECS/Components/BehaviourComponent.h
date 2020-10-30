#pragma once

#include "Core/Core.h"
#include "Core/Scripting/OutValues.h"

#include <string>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace SmolEngine
{
	class Actor;

	struct BehaviourComponent
	{
		BehaviourComponent();

		/// Data

		size_t ID = 0;

		///
		
		std::vector<OutValue> OutValues;

		///

		std::string SystemName = "";

		///

		Ref<Actor> Actor = nullptr;

	private:

		friend class EditorLayer;

		friend class WorldAdmin;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(ID, SystemName, OutValues);
		}
	};
}