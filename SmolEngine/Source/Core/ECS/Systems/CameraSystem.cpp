#include "stdafx.h"
#include "CameraSystem.h"
#include "Core/EventHandler.h"
#include "Core/Events/ApplicationEvent.h"

#include "Core/ECS/Components/Singletons/FramebufferSComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SmolEngine
{
	CameraSystem::CameraSystem()
	{

	}

	void CameraSystem::CalculateView(CameraBaseTuple& tuple)
	{
		const auto& camera = tuple.Camera;

		SetProjection(tuple, -camera.AspectRatio * camera.ZoomLevel,
			camera.AspectRatio * camera.ZoomLevel, -camera.ZoomLevel, camera.ZoomLevel, camera.zNear, camera.zFar);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), tuple.Transform.WorldPos) * glm::rotate(glm::mat4(1.0f),
			glm::radians(tuple.Transform.Rotation.x), glm::vec3(0, 0, 1) );

		tuple.Camera.ViewMatrix = glm::inverse(transform);
		tuple.Camera.ViewProjectionMatrix = tuple.Camera.ProjectionMatrix * tuple.Camera.ViewMatrix;
	}

	void CameraSystem::SetProjection(CameraBaseTuple& tuple, float left, float right, float buttom, float top, float zNear, float zFar)
	{
		auto& camera = tuple.Camera;

		camera.ProjectionMatrix = glm::ortho(left, right, buttom, top, zNear, zFar);

		camera.ViewProjectionMatrix = camera.ProjectionMatrix * camera.ViewMatrix;
	}

	void CameraSystem::OnResize(CameraBaseTuple& tuple, float width, float height)
	{
		auto& camera = tuple.Camera;

		camera.AspectRatio = width / height;

		SetProjection(tuple, -camera.AspectRatio * camera.ZoomLevel,
			camera.AspectRatio * camera.ZoomLevel, -camera.ZoomLevel, camera.ZoomLevel, camera.zNear, camera.zFar);
	}

	void CameraSystem::OnEvent(CameraBaseTuple& tuple, Event& e)
	{
		if (e.m_EventType == (int)EventType::S_WINDOW_RESIZE)
		{
			auto& winResize = static_cast<WindowResizeEvent&>(e);

			OnResize(tuple, (float)winResize.GetHeight(), (float)winResize.GetWidth());
		}
	}
}