#pragma once
#include "Core/Core.h"

#include "UI/UIElement.h"
#include "UI/UIButton.h"
#include "UI/UITextLabel.h"
#include "ECS/Components/BaseComponent.h"

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/json.hpp>

CEREAL_REGISTER_TYPE(SmolEngine::UIButton);
CEREAL_REGISTER_POLYMORPHIC_RELATION(SmolEngine::UIElement, SmolEngine::UIButton);

CEREAL_REGISTER_TYPE(SmolEngine::UITextLabel);
CEREAL_REGISTER_POLYMORPHIC_RELATION(SmolEngine::UIElement, SmolEngine::UITextLabel);

namespace SmolEngine
{
	struct CanvasComponent: public BaseComponent
	{
		CanvasComponent();

		CanvasComponent(uint32_t id)
			:BaseComponent(id) {}

		/// Data

		std::unordered_map<size_t, Ref<UIElement>> Elements;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Elements, ComponentID);
		}
	};
}