#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#include <string>
#include <vector>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace SmolEngine
{
	struct HeadComponent: public BaseComponent
	{
		HeadComponent() = default;
		HeadComponent(uint32_t id)
			: BaseComponent(id) {}
		HeadComponent(const std::string& name, const std::string tag, uint32_t id)
			:Name(name), Tag(tag), ActorID(id) {}


		Actor*                Parent = nullptr;
		bool                  bEnabled = true;
		bool                  bShowComponentUI = false;
		uint32_t              ActorID = 0;
		uint32_t              ParentID = 0;
		uint32_t              ComponentsCount = 0;
		std::string           Name = "";
		std::string           Tag = "";
		std::vector<uint32_t> ChildsIDs;
		std::vector<Actor*>   Childs;

	private:

		friend class EditorLayer;
		friend class WorldAdmin;
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(ActorID, Name, Tag, bEnabled, bShowComponentUI, ChildsIDs, ParentID, ComponentID, ComponentsCount);
		}
	};
}