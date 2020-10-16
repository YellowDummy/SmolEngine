#pragma once

#include "Core/Core.h"
#include "Core/UI/UIElement.h"
#include "Core/UI/UIButton.h"
#include "Core/UI/UITextLabel.h"

#include <unordered_map>
#include <glm/glm.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/unordered_map.hpp>


#include <cereal/archives/json.hpp>

CEREAL_REGISTER_TYPE(SmolEngine::UIButton);
CEREAL_REGISTER_POLYMORPHIC_RELATION(SmolEngine::UIElement, SmolEngine::UIButton);

CEREAL_REGISTER_TYPE(SmolEngine::UITextLabel);
CEREAL_REGISTER_POLYMORPHIC_RELATION(SmolEngine::UIElement, SmolEngine::UITextLabel);

//TODO:: Add Alignment

namespace SmolEngine
{
	class Event;

	class UICanvas
	{
	public:

		UICanvas() = default;

		/// Main

		void OnUpdate();

		void OnEvent(Event& e);

		/// Elements

		Ref<UIElement> AddElement(UIElementType type);

		void DeleteElement(size_t id);

		//Rendering

		void DrawAllElements(const glm::vec3& cameraPos, const float zoomLevel) const;

		void DrawElement(const size_t id, const glm::vec3& cameraPos, const float zoomLevel) const;

		// Getters

		Ref<UIButton> GetButton(size_t index) const;

		Ref<UITextLabel> GetTextLabel(size_t index) const;

		Ref<UIElement> GetElement(const size_t index) const;

	private:

		/// Events

		void ReloadElements();

		bool OnMouseClick(Event& e);

		bool OnMouseMove(Event& e);

	private:

		std::unordered_map<size_t, Ref<UIElement>> m_Elements;

	private:

		friend class cereal::access;
		friend class Scene;
		friend class EditorLayer;
		friend class UILayer;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_Elements);
		}

	};
}