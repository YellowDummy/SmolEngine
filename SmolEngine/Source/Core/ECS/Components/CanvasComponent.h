#pragma once
#include "Core/Core.h"

#include "Core/UI/UIElement.h"
#include "Core/UI/UIButton.h"
#include "Core/UI/UITextLabel.h"

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
	struct CanvasComponent
	{
		CanvasComponent();

		///

		std::unordered_map<size_t, Ref<UIElement>> Elements;

	private:

		friend class EditorLayer;

		friend class WorldAdmin;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Elements);
		}
	};
}