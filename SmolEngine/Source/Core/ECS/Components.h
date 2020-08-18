#pragma once

#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Camera.h"
#include "Core/Time.h"
#include "Core/Core.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	struct TransfromComponent
	{
		glm::vec3 WorldPos = glm::vec3(1.0f);
		glm::vec2 Scale = glm::vec2(1.0f);;
		float Rotation = 0;

		TransfromComponent() = default;
		TransfromComponent(const TransfromComponent&) = default;
		TransfromComponent(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation)
			:WorldPos(worldPos), Scale(scale), Rotation(rotation) {}

	};

	struct Texture2DComponent
	{
		bool Enabled = true;
		glm::vec4 Color = glm::vec4(1.0f);
		Ref<Texture2D> Texture;

		Texture2DComponent() = default;
		Texture2DComponent(const Texture2DComponent&) = default;
		Texture2DComponent(const glm::vec4& color): Color(color) {}
		Texture2DComponent(const Ref<Texture2D> texture, const glm::vec4& color = glm::vec4(1.0f))
			:Texture(texture), Color(color) {}
	};

	struct CameraComponent
	{
		bool Enabled = true;
		Ref<CameraController> Camera = std::make_shared<CameraController>(1280.0f / 720.0f);
		glm::mat4 Transform = glm::mat4(1.0f);
	};

	struct ScriptComponent
	{
		virtual void Start() = 0;
		virtual void OnUpdate(DeltaTime deltaTime) = 0;
		virtual void OnDestroy() = 0;
	};


}