#pragma once

#include "Core/Time.h"
#include "Core/EventHandler.h"
#include "Core/Renderer/Framebuffer.h"

#include <memory>
#include <glm/glm.hpp>
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>

namespace SmolEngine
{
	class OrthographicCamera
	{
	public:

		OrthographicCamera() = default;

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

		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::vec3 m_CameraPos = { 0.0f, 0.0f, 0.0f };

		float m_CameraRotation = 0;

		friend class cereal::access;
		friend class WorldAdmin;
	};

	class CameraController
	{
	public:

		CameraController();

		CameraController(float aspectRatio, bool roatationEnabled = false);

		~CameraController() = default;

		void CalculateView();

		void SetZoom(const float value) { m_ZoomLevel = value; CalculateView(); }

		void SetTransform(const glm::vec3& wolrdPos);

		const float GetZoom() { return m_ZoomLevel; }

		Ref<OrthographicCamera> GetCamera() {return m_Camera;}

		const Ref<Framebuffer> GetFramebuffer() { return m_FrameBuffer; }

		void Reload();

	private:

		void OnResize(float width, float height);

		void OnSceneEvent(Event& event);

		bool IsWindowResized(Event& event);

	private:

		Ref<OrthographicCamera> m_Camera = nullptr;
		Ref<Framebuffer> m_FrameBuffer = nullptr;

		float m_ZoomLevel = 4.0f;
		float m_AspectRatio = 0.0f;

	private:

		friend class WorldAdmin;
		friend class EditorLayer;
		friend class AnimationPanel;
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_ZoomLevel, m_AspectRatio);
		}
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

		Ref<OrthographicCamera> GetCamera() { return m_Camera; }

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
		Ref<OrthographicCamera> m_Camera = nullptr;

	private:

		friend class WorldAdmin;
		friend class EditorLayer;
	};

}