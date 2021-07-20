#pragma once
#include "Core/Core.h"
#include "ECS/Actor.h"
#include "ECS/Components/BaseComponent.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct CSharpScriptComponent : public BaseComponent
	{
		CSharpScriptComponent() = default;
		CSharpScriptComponent(uint32_t id)
			: BaseComponent(id) {}

		Ref<Actor>  Actor = nullptr;
		void*       ClassInstance = nullptr;
		std::string ClassName = "";

	private:
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(ClassName, Actor, ComponentID);
		}
	};
}