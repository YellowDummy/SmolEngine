#pragma once
#include "Core/Core.h"
#include "Core/UI/UIElement.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	class UIElement;

	class UITextLabel;

	struct CanvasComponent;

	///

	class UISystem
	{
	public:

		UISystem() = default;

		/// Main

		static void OnUpdate(CanvasComponent& canvas);

		static void OnEvent(CanvasComponent& canvas, Event& e);

		/// Elements

		static Ref<UIElement> AddElement(CanvasComponent& canvas, UIElementType type);

		static bool DeleteElement(CanvasComponent& canvas, size_t id);

		//Rendering

		static void DrawAllElements(const CanvasComponent& canvas, const glm::vec3& cameraPos, const float zoomLevel);

		// Getters

		static Ref<UIButton> GetButton(const CanvasComponent& canvas, size_t index);

		static Ref<UITextLabel> GetTextLabel(const CanvasComponent& canvas, size_t index);

		static Ref<UIElement> GetElement(const CanvasComponent& canvas, const size_t index);

	private:

		/// Events

		static void ReloadElements(CanvasComponent& canvas);

		static bool OnMouseClick(CanvasComponent& canvas, Event& e);

		static bool OnMouseMove(CanvasComponent& canvas, Event& e);


	private:

		friend class cereal::access;

		friend class Scene;

		friend class EditorLayer;

		friend class UILayer;
	};
}