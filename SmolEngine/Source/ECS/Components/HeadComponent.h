#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#include <string>
#include <cereal/cereal.hpp>


namespace SmolEngine
{
	struct HeadComponent: public BaseComponent
	{
		HeadComponent();

		HeadComponent(uint32_t id)
			: BaseComponent(id) {}

		HeadComponent(const std::string& name, const std::string tag, uint32_t id);

		/// Data

		std::string Name = "";
		std::string Tag = "";

		uint32_t ID = 0;
		bool IsEnabled = true;

	private:

		friend class EditorLayer;
		friend class WorldAdmin;
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(ID, Name, Tag, IsEnabled, ComponentID);
		}
	};
}