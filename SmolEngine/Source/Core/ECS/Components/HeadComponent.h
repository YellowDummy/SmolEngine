#pragma once
#include "Core/Core.h"

#include <string>
#include <cereal/cereal.hpp>


namespace SmolEngine
{
	struct HeadComponent
	{
		HeadComponent();

		HeadComponent(const std::string& name, const std::string tag, size_t id);

		///

		std::string Name = "";

		std::string Tag = "";

		///

		size_t ID = 0;

		///

		bool IsEnabled = true;

	private:

		friend class EditorLayer;

		friend class WorldAdmin;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(ID, Name, Tag, IsEnabled);
		}
	};
}