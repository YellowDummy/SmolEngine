#include "stdafx.h"
#include "Camera.h"
#include "Core/Input.h"
#include "Core/SLog.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"
#include "Renderer/FramebufferSpecification.h"

#include <glm/gtc/matrix_transform.hpp>

namespace SmolEngine
{
	OrthographicCamera::OrthographicCamera(float left, float right, float buttom, float top, float zNear, float zFar)
		:m_ProjectionMatrix(glm::ortho(left, right, buttom, top, zNear, zFar)), 
		m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	OrthographicCamera::OrthographicCamera(float zoomLevel, float aspectRatio)
		:m_ProjectionMatrix(glm::ortho(-aspectRatio * zoomLevel, aspectRatio* zoomLevel, -zoomLevel, zoomLevel)),
		m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float buttom, float top, float zNear, float zFar)
	{
		m_ProjectionMatrix = glm::ortho(left, right, buttom, top, zNear, zFar);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_CameraPos) * glm::rotate(glm::mat4(1.0f), glm::radians(m_CameraRotation), glm::vec3(0,0,1));
		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	/// Camera Controls

	CameraController::CameraController()
	{
		float aspectRatio = (float)Application::GetApplication().GetWindowHeight() / (float)Application::GetApplication().GetWindowWidth();
		m_Camera = std::make_shared<OrthographicCamera>(-aspectRatio * m_ZoomLevel, aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		FramebufferSpecification spec;
		spec.Width = Application::GetApplication().GetWindowWidth();
		spec.Height = Application::GetApplication().GetWindowHeight();
		m_FrameBuffer = Framebuffer::Create(spec);
	}

	CameraController::CameraController(float aspectRatio, bool roatationEnabled)
		:m_Camera(std::make_shared<OrthographicCamera>(-aspectRatio * m_ZoomLevel, aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)),
		m_AspectRatio(aspectRatio)
	{
		FramebufferSpecification spec;
		spec.Width = Application::GetApplication().GetWindowWidth();
		spec.Height = Application::GetApplication().GetWindowHeight();
		m_FrameBuffer = Framebuffer::Create(spec);
	}

	void CameraController::CalculateView()
	{
		m_Camera->SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	void CameraController::SetTransform(const glm::vec3& wolrdPos)
	{
		m_Camera->SetPosition(wolrdPos);
	}

	void CameraController::Reload()
	{
		auto& app = Application::GetApplication();

		const float width = app.GetWindow().GetHeight();
		const float height = app.GetWindow().GetWidth();
		float aspectRatio = height / width;


		m_Camera = std::make_shared<OrthographicCamera>(-aspectRatio * m_ZoomLevel, aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		FramebufferSpecification spec;
		spec.Width = height;
		spec.Height = width;
		m_FrameBuffer = Framebuffer::Create(spec);

		OnResize(height, width);
	}

	void CameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		CalculateView();
	}

	void CameraController::OnSceneEvent(Event& event)
	{
		S_BIND_EVENT_TYPE(CameraController, IsWindowResized, EventType::S_WINDOW_RESIZE, event);
	}

	bool CameraController::IsWindowResized(Event& event)
	{
		auto& e = static_cast<WindowResizeEvent&>(event);
		OnResize(e.GetHeight(), (float)e.GetWidth());

		return false;
	}
}