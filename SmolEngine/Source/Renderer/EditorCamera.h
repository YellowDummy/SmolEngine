#pragma once
#include "Core/Core.h"
#include "Core/Time.h"
#include "Core/EventHandler.h"
#include <glm/glm.hpp>

namespace SmolEngine
{
	class Framebuffer;

	enum class CameraType : uint16_t
	{
		Perspective,
		Ortho
	};

	struct EditorCameraCreateInfo
	{
		float             FOV = 75.0f;
		float             NearClip = 0.1f;
		float             FarClip = 1000.0f;
		float             Speed = 1.0f;
		float             Pitch = 0.0f;
		float             Yaw = 0.0f;

		glm::vec3         WorldPos = glm::vec3(0, 0, 0);
		CameraType        Type = CameraType::Perspective;
	};

	class EditorCamera
	{
	public:

		EditorCamera(EditorCameraCreateInfo* createInfo = nullptr);

		// Main

		void OnUpdate(DeltaTime delta);

		void OnEvent(Event& event);

		// Setters

		void SetDistance(float distance) { m_Distance = distance; }

		void SetViewportSize(float width, float height);

		void SetCameraType(CameraType type);

		// Getters

		const glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		const glm::mat4& GetProjection() const { return m_Projection; }

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

		const glm::vec3& GetPosition() const { return m_Position; }

		const glm::vec3& GetFPoint() const { return m_FocalPoint; }

		float GetDistance() const { return m_Distance; }

		float GetPitch() const { return m_Pitch; }

		float GetYaw() const { return m_Yaw; }

		float GetNearClip() const { return m_NearClip; }

		float GetFarClip() const { return m_FarClip; }

		glm::vec3 GetForwardDirection() const;

		glm::vec3 GetRightDirection() const;

		glm::vec3 GetUpDirection() const;

		glm::quat GetOrientation() const;

		const CameraType GetType() const ;

	private:

		// Calculations

		void UpdateProjection();

		void UpdateViewPerspective();

		void UpdateViewOrtho();

		float RotationSpeed() const;

		float ZoomSpeed() const;

		std::pair<float, float> PanSpeed() const;

		glm::vec3 CalculatePosition() const;

		// Mouse

		void MousePan(const glm::vec2& delta);

		void MouseRotate(const glm::vec2& delta);

		void MouseZoom(float delta);

		// Events

		bool OnMouseScroll(Event& e);

		bool OnResize(Event& e);

		bool OnResize(uint32_t width, uint32_t height);

	private:

		glm::mat4                       m_Projection = glm::mat4(0.0f);
		glm::mat4                       m_ViewMatrix = glm::mat4(0.0f);
		glm::vec3                       m_Position = glm::vec3(0.0f);
		glm::vec3                       m_FocalPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec2                       m_InitialMousePosition = glm::vec2(0.0f);
						                
		float                           m_FOV = 75.0f;
		float                           m_AspectRatio = 1.778f;
		float                           m_NearClip = 0.1f;
		float                           m_FarClip = 1000.0f;
		float                           m_Distance = 6.0f;
		float                           m_Pitch = 0.0f, m_Yaw = 0.0f;
		float                           m_ViewportWidth = 1280;
		float                           m_ViewportHeight = 720;
		float                           m_RotationSpeed = 0.8f;
		float                           m_MaxZoomSpeed = 100.0f;
		float                           m_Speed = 2.0f;

		CameraType                      m_Type = CameraType::Perspective;

	private:

		friend class EditorLayer;
	};
}