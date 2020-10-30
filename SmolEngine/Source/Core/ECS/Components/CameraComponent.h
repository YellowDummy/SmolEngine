#pragma once

#include "Core/Core.h"

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct CameraComponent
	{
		CameraComponent();

		///

		glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);

		glm::mat4 ProjectionMatrix = glm::mat4(1.0f);

		glm::mat4 ViewMatrix = glm::mat4(1.0f);

		///

		float AspectRatio = 1.0;

		float ZoomLevel = 6.0f;

		float zNear = -1.0f;

		float zFar = 1.0f;

		///

		bool isPrimaryCamera = false;

		bool isEnabled = true;

		bool ShowCanvasShape = true;

	private:

		friend class EditorLayer;

		friend class WorldAdmin;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(AspectRatio, ZoomLevel, isPrimaryCamera, isEnabled, zNear, zFar, ShowCanvasShape);
		}
	};
}