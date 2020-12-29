#pragma once

#include "Core/Core.h"
#include "Core/ECS/Components/BaseComponent.h"

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct CameraComponent: public BaseComponent
	{
		CameraComponent();

		CameraComponent(uint32_t id)
			:BaseComponent(id) {}

		/// Data

		glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 ViewMatrix = glm::mat4(1.0f);

		float AspectRatio = 1.0;
		float ZoomLevel = 6.0f;
		float zNear = -1.0f;
		float zFar = 1.0f;

		bool isPrimaryCamera = false;
		bool isEnabled = true;
		bool ShowCanvasShape = true;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(AspectRatio, ZoomLevel, isPrimaryCamera, isEnabled, zNear, zFar, ShowCanvasShape, ComponentID);
		}
	};
}