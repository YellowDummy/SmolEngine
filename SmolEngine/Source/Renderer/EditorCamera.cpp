#include "stdafx.h"
#include "EditorCamera.h"
#include "Core/Input.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"
#include "Renderer/Framebuffer.h"
#include "Core/Application.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace SmolEngine
{
	EditorCamera::EditorCamera(EditorCameraCreateInfo* createInfo)

	{
		if (createInfo)
		{
			m_FOV = createInfo->FOV;
			m_AspectRatio = createInfo->AspectRation;
			m_NearClip = createInfo->NearClip;
			m_FarClip = createInfo->FarClip;
		}

		FramebufferSpecification spec;
		spec.IsTargetsSwapchain = createInfo ? createInfo->IsFramebufferTargetsSwapchain: false;
		spec.Width = Application::GetApplication().GetWindowWidth();
		spec.Height = Application::GetApplication().GetWindowHeight();
		m_FrameBuffer = Framebuffer::Create(spec);

		//m_Projection = glm::ortho(-aspectRation * m_Distance, aspectRation * m_Distance, -m_Distance, m_Distance);
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
		UpdateView();
	}

	void EditorCamera::OnUpdate(DeltaTime delta)
	{

		if (Input::IsKeyPressed(Key::W))
			m_FocalPoint += GetForwardDirection() / 100.0f;

		if (Input::IsKeyPressed(Key::S))
			m_FocalPoint -= GetForwardDirection() / 100.0f;

		if (Input::IsKeyPressed(Key::D))
			m_FocalPoint += GetRightDirection() / 175.0f;

		if (Input::IsKeyPressed(Key::A))
			m_FocalPoint -= GetRightDirection() / 175.0f;

		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		glm::vec2 deltaT = (mouse - m_InitialMousePosition) * 0.003f;
		m_InitialMousePosition = mouse;

		if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
			MousePan(deltaT);

		if (Input::IsKeyPressed(Key::LeftAlt))
		{

			if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
				MouseRotate(deltaT);

			if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
				MouseZoom(deltaT.y);
		}

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& event)
	{
		if (event.m_EventType == (int)EventType::S_MOUSE_SCROLL)
		{
			OnMouseScroll(event);
		}
		if (event.m_EventType == (int)EventType::S_WINDOW_RESIZE)
		{
			OnResize(event);
		}
	}

	inline void EditorCamera::SetViewportSize(float width, float height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height; 
		UpdateProjection();
	}

	void EditorCamera::SetCameraType(CameraType type)
	{
		m_Type = type;
		switch (m_Type)
		{
		case CameraType::Perspective:
		{
			m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
			break;
		}
		case CameraType::Ortho:
		{
			m_Projection = glm::ortho(-m_AspectRatio * m_Distance, m_AspectRatio * m_Distance, -m_Distance, m_Distance);
			break;
		}
		default:
			break;
		}
		UpdateView();
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	Ref<Framebuffer>& EditorCamera::GetFramebuffer()
	{
		return m_FrameBuffer;
	}

	const CameraType EditorCamera::GetType() const
	{
		return m_Type;
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::UpdateView()
	{
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.0f;
		}
	}

	bool EditorCamera::OnMouseScroll(Event& e)
	{
		auto& m_e = static_cast<MouseScrollEvent&>(e);
		float delta = m_e.GetYoffset() * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	bool EditorCamera::OnResize(Event& e)
	{
		auto& res_e = static_cast<WindowResizeEvent&>(e);

		m_AspectRatio = res_e.GetHeight() / res_e.GetWidth();

		m_FrameBuffer->OnResize(res_e.GetWidth(), res_e.GetHeight());
		return false;
	}

	bool EditorCamera::OnResize(uint32_t width, uint32_t height)
	{
		m_FrameBuffer->OnResize(width, height);
		return false;
	}

	float EditorCamera::RotationSpeed() const
	{
		return m_RotationSpeed;
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, m_MaxZoomSpeed);
		return speed;
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}
}