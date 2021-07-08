#include "stdafx.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"
#include "ECS/ComponentsCore.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SmolEngine
{
	void CameraSystem::CalculateView(CameraComponent* camera, TransformComponent* transform)
	{
		glm::vec2 size = Engine::GetEngine()->GetViewPortSize();
		camera->AspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);

		switch (camera->eType)
		{
		case CameraComponentType::Ortho:        UpdateViewOrtho(camera, transform); break;
		case CameraComponentType::Perspective:  UpdateViewPerspective(camera, transform); break;
		}
	}

	void CameraSystem::UpdateViewPerspective(CameraComponent* camera, TransformComponent* transform)
	{
		glm::quat orientation = glm::quat(transform->Rotation);
		camera->ViewMatrix = glm::translate(glm::mat4(1.0f), transform->WorldPos) * glm::toMat4(orientation);
		camera->ViewMatrix = glm::inverse(camera->ViewMatrix);

		camera->ProjectionMatrix = glm::perspective(glm::radians(camera->FOV), camera->AspectRatio, camera->zNear, camera->zFar);
	}

	void CameraSystem::UpdateViewOrtho(CameraComponent* camera, TransformComponent* transform)
	{
		glm::mat4 mat = glm::translate(glm::mat4(1.0f), transform->WorldPos) * glm::rotate(glm::mat4(1.0f), glm::radians(transform->Rotation.x), glm::vec3(0, 0, 1));
		camera->ViewMatrix = glm::inverse(mat);
		camera->ProjectionMatrix = glm::ortho(-camera->AspectRatio * camera->ZoomLevel, camera->AspectRatio * camera->ZoomLevel, -camera->ZoomLevel, camera->ZoomLevel, camera->zNear, camera->zFar);
	}

	void CameraSystem::OnResize(uint32_t width, uint32_t height)
	{
		entt::registry* reg = WorldAdminStateSComponent::GetSingleton()->m_CurrentRegistry;
		const auto& group = reg->view<CameraComponent, TransformComponent>();
		for (const auto& entity : group)
		{
			auto& [camera, transform] = group.get<CameraComponent, TransformComponent>(entity);
			camera.AspectRatio = static_cast<float>(width) / static_cast<float>(height);
			CalculateView(&camera, &transform);
		}
	}

}