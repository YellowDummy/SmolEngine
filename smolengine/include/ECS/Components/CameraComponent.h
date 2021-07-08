#pragma once

#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	enum class CameraComponentType : int
	{
		Perspective,
		Ortho
	};

	struct CameraComponent: public BaseComponent
	{
		CameraComponent()
		{
			AspectRatio = (float)Engine::GetEngine()->GetWindowWidth() / (float)Engine::GetEngine()->GetWindowHeight();
		}

		CameraComponent(uint32_t id)
			:BaseComponent(id) 
		{
			AspectRatio = (float)Engine::GetEngine()->GetWindowWidth() / (float)Engine::GetEngine()->GetWindowHeight();
		}

		glm::mat4            ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4            ViewMatrix = glm::mat4(1.0f);
		float                AspectRatio = 1.0;
		float                ZoomLevel = 6.0f;
		float                zNear = 0.1f;
		float                zFar = 1000.0f;
		int                  ImGuiType = 0;
		float                FOV = 75.0f;
		bool                 bPrimaryCamera = false;
		CameraComponentType  eType = CameraComponentType::Perspective;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(AspectRatio, ZoomLevel, bPrimaryCamera, eType, zNear, zFar, FOV, ImGuiType, ComponentID);
		}
	};
}