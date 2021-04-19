#include "stdafx.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/ComponentsCore.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SmolEngine
{
	CameraSystem::CameraSystem()
	{

	}

	void CameraSystem::CalculateView(CameraComponent* camera, TransformComponent* tranform)
	{
		SetProjection(camera, -camera->AspectRatio * camera->ZoomLevel,
			camera->AspectRatio * camera->ZoomLevel, -camera->ZoomLevel, camera->ZoomLevel, camera->zNear, camera->zFar);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), tranform->WorldPos) * glm::rotate(glm::mat4(1.0f),
			glm::radians(tranform->Rotation.x), glm::vec3(0, 0, 1) );

		camera->ViewMatrix = glm::inverse(transform);
		camera->ViewProjectionMatrix = camera->ProjectionMatrix * camera->ViewMatrix;
	}

	void CameraSystem::SetProjection(CameraComponent* camera, float left, float right, float buttom, float top, float zNear, float zFar)
	{
		camera->ProjectionMatrix = glm::ortho(left, right, buttom, top, zNear, zFar);
		camera->ViewProjectionMatrix = camera->ProjectionMatrix * camera->ViewMatrix;
	}

	void CameraSystem::OnResize(entt::registry& registry, float width, float height)
	{
		const auto& group = registry.view<CameraComponent>();
		for (const auto& entity : group)
		{
			auto& camera = group.get<CameraComponent>(entity);
			camera.AspectRatio = width / height;
			SetProjection(&camera, -camera.AspectRatio * camera.ZoomLevel,
				camera.AspectRatio * camera.ZoomLevel, -camera.ZoomLevel, camera.ZoomLevel, camera.zNear, camera.zFar);
		}
	}

	void CameraSystem::OnEvent(entt::registry& registry, Event& e)
	{

	}
}