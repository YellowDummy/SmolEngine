#include "stdafx.h"
#include "Camera.h"
#include "Core/Input.h"
#include "Core/SLog.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/ApplicationEvent.h"

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



	///----------------------------------------------------------------CAMERA-CONTROLLER--------------------------------------------------------------------------------------------//

	CameraController::CameraController(float aspectRatio, bool roatationEnabled)
		:m_Camera(-aspectRatio * m_ZoomLevel, aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
		m_CameraRotationSpeed(roatationEnabled),
		m_AspectRatio(aspectRatio)
	{

	}

	void CameraController::CalculateView()
	{
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	void CameraController::OnUpdate(DeltaTime deltaTime, glm::vec3& wolrdPos)
	{

		if (Input::IsKeyPressed(S_KEY_D))
		{
			wolrdPos.x += m_CameraSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(S_KEY_A))
		{
			wolrdPos.x -= m_CameraSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(S_KEY_W))
		{
			wolrdPos.y += m_CameraSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(S_KEY_S))
		{
			wolrdPos.y -= m_CameraSpeed * deltaTime;
		}

		m_Camera.SetPosition(wolrdPos);
		m_CameraSpeed = m_ZoomLevel;
	}

	void CameraController::OnEvent(Event& event)
	{
		S_BIND_EVENT_TYPE(CameraController, IsMouseScrolled, EventType::S_MOUSE_SCROLL, event);
		S_BIND_EVENT_TYPE(CameraController, IsWindowResized, EventType::S_WINDOW_RESIZE, event);
	}

	void CameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		CalculateView();
	}

	bool CameraController::IsMouseScrolled(Event& event)
	{
		auto& e = static_cast<MouseScrollEvent&>(event);
		m_ZoomLevel -= e.GetYoffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		CalculateView();

		return false;
	}

	bool CameraController::IsWindowResized(Event& event)
	{
		auto& e = static_cast<WindowResizeEvent&>(event);
		OnResize(e.GetHeight(), (float)e.GetWidth());

		return false;
	}
}