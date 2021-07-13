#pragma once
#include "Core/Core.h"
#include "TexturesLoader.h"

#include <glm/glm.hpp>
#include <Frostium3D/Libraries/imgizmo/src/ImGuizmo.h>
#include <Frostium3D/Primitives/Framebuffer.h>

namespace SmolEngine
{
	class Actor;
	class EditorLayer;

	class ViewPort
	{
	public:
		ViewPort();
		virtual ~ViewPort() = default;

		void                SetActive(bool value) { m_Active = value; }
		const glm::vec2&    GetSize() const { return m_ViewPortSize; }
		bool                IsFocused() const { return m_Focused; }
		bool                IsActive() const { return m_Active; }
		virtual void        Render() {}
		virtual void        Draw() = 0;

	private:
		WorldAdmin*         m_World = nullptr;
		bool                m_Focused = false;
		bool                m_Active = false;
		glm::vec2           m_ViewPortSize = glm::vec2(0);

		friend class SceneView;
		friend class GameView;
	};

	class SceneView : public ViewPort
	{
	public:
		SceneView(EditorLayer* editor);

		void                  Draw() override;

	private:
		bool                  m_GizmoEnabled = true;
		bool                  m_SnapEnabled = false;
		TexturesLoader*       m_TexturesLoader = nullptr;
		EditorLayer*          m_Editor = nullptr;
		ImGuizmo::OPERATION   m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	};

	class GameView : public ViewPort
	{
	public:
		GameView();

		void                  Draw() override;
		void                  Render() override;

		Framebuffer           m_PreviewFramebuffer = {};
	};
}