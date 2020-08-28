#pragma once

#include "Core/Time.h"
#include "Core/EventHandler.h"
#include "Core/Renderer/Framebuffer.h"
#include <memory>
#include <glm/glm.hpp>

namespace SmolEngine
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float buttom, float top, float zNear = -1.0f, float zFar = 1.0f);
		OrthographicCamera(float zoomLevel, float aspectRation);


		void SetPosition(const glm::vec3& position) { m_CameraPos = position; RecalculateViewMatrix(); }
		void SetRotation(const float rotation) { m_CameraRotation = rotation; RecalculateViewMatrix(); }
		void SetProjection(float left, float right, float buttom, float top, float zNear = -1.0f, float zFar = 1.0f);

		inline const float GetRotation() { return m_CameraRotation; }
		inline const glm::vec3& GetPosistion() const { return m_CameraPos; }
		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	private:
		void RecalculateViewMatrix();

	private:
		glm::mat4 m_ViewProjectionMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::vec3 m_CameraPos = { 0.0f, 0.0f, 0.0f };

		float m_CameraRotation = 0;
	};

	class CameraController
	{
	public:
		CameraController(float aspectRatio, bool roatationEnabled = false);
		virtual ~CameraController() = default;

		void CalculateView();

		void SetZoom(float value) { m_ZoomLevel = value; CalculateView(); }
		void SetTransform(glm::vec3& wolrdPos);

		const float GetZoom() { return m_ZoomLevel; }

		OrthographicCamera& GetCamera() {return m_Camera;}
		const OrthographicCamera& GetCamera() const { return m_Camera; }

	private:
		void OnResize(float width, float height);
		void OnSceneEvent(Event& event);
		bool IsWindowResized(Event& event);

	private:
		Ref<Framebuffer> m_FrameBuffer;
		float m_ZoomLevel = 1.0f;
		float m_AspectRatio;
		OrthographicCamera m_Camera;

		friend class Scene;
		friend class EditorLayer;
	};

	class EditorCameraController
	{
	public:
		EditorCameraController(float aspectRatio, bool roatationEnabled = false);
		virtual ~EditorCameraController() = default;

		void CalculateView();
		void OnUpdate(DeltaTime deltaTime);
		void OnEvent(Event& event);
		void SetZoom(float value) { m_ZoomLevel = value; CalculateView(); }
		void OnResize(float width, float height);

		const float GetZoom() { return m_ZoomLevel; }

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

	private:
		bool IsMouseScrolled(Event& event);
		bool IsWindowResized(Event& event);

	private:
		glm::vec3 m_WorldPos;
		bool m_RoatationEnabled;
		float m_ZoomLevel = 1.0f;
		float m_AspectRatio;
		float m_CameraSpeed = 3.0f, m_CameraRotationSpeed = 1.0f;

		Ref<Framebuffer> m_FrameBuffer;
		OrthographicCamera m_Camera;

		friend class Scene;
		friend class EditorLayer;
	};

}