#pragma once
#include "Core/Core.h"
#include "Core/UI/UIElement.h"

#include <glm/glm.hpp>
#include <entt.hpp>

namespace SmolEngine
{
	class UIElement;

	class UIButton;

	class UITextLabel;

	struct CanvasComponent;

	///

	class UISystem
	{
	public:

		UISystem() = default;

		/// Main

		static void OnUpdate(entt::registry& registry);

		static void OnEvent(entt::registry& registry, Event& e);

		/// Elements

		static Ref<UIElement> AddElement(CanvasComponent& canvas, UIElementType type);

		static bool DeleteElement(CanvasComponent& canvas, size_t id);

		//Rendering

		static void DrawAllElements(const CanvasComponent& canvas, const glm::vec3& cameraPos, const float zoomLevel);

		// Getters

		static Ref<UIButton> GetButton(const CanvasComponent& canvas, size_t index);

		static Ref<UITextLabel> GetTextLabel(const CanvasComponent& canvas, size_t index);

		static Ref<UIElement> GetElement(const CanvasComponent& canvas, const size_t index);

		/// Events

		static void ReloadElements(CanvasComponent& canvas);

	private:

		static bool OnMouseClick(CanvasComponent& canvas, Event& e);

		static bool OnMouseMove(CanvasComponent& canvas, Event& e);


	private:

		friend class cereal::access;
		friend class WorldAdmin;
		friend class EditorLayer;
		friend class UILayer;
	};
}