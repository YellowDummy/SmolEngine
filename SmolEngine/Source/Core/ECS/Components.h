#pragma once

#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Camera.h"
#include "Core/Application.h"
#include "Core/Time.h"
#include "Core/Core.h"

#include "Core/ECS/ScriptableObject.h"
#include "Core/Scripting/Jinx.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <tuple>
#include <any>
#include <Jinx.hpp>

namespace SmolEngine
{
	class Actor;

	struct TransfromComponent
	{
		glm::vec3 WorldPos = glm::vec3(1.0f);
		glm::vec2 Scale = glm::vec2(1.0f);;
		float Rotation = 0;

		TransfromComponent(const TransfromComponent&) = default;
		TransfromComponent(const glm::vec3& worldPos = glm::vec3(1.0f), const glm::vec2& scale = glm::vec2(1.0f), const float rotation = 0)
			:WorldPos(worldPos), Scale(scale), Rotation(rotation) {}

	};

	struct Texture2DComponent
	{
		bool Enabled = true;
		glm::vec4 Color = glm::vec4(1.0f);
		Ref<Texture2D> Texture = nullptr;

		Texture2DComponent() = default;
		Texture2DComponent(const Texture2DComponent&) = default;
		Texture2DComponent(const glm::vec4& color): Color(color) {}
		Texture2DComponent(const Ref<Texture2D> texture, const glm::vec4& color = glm::vec4(1.0f))
			:Texture(texture), Color(color) {}
	};

	struct CameraComponent
	{
		bool Enabled = true;
		float aspectRatio = Application::GetApplication().GetWindowHeight() / Application::GetApplication().GetWindowWidth();
		Ref<CameraController> Camera = nullptr;

		CameraComponent()
		{
			Camera = std::make_shared<CameraController>(aspectRatio);
			Camera->SetZoom(4.0f);
		}

	};

	struct ScriptComponent
	{
		bool Enabled = true;

		template<typename T>
		std::shared_ptr<T> SetScript(Ref<Actor> actor)
		{
			Script = std::dynamic_pointer_cast<ScriptableObject>(std::make_shared<T>(actor));
			return std::make_shared<T>(actor);
		}

		void OnUpdate(DeltaTime deltaTime)
		{
			if (Script != nullptr)
			{
				if (Script->Enabled == false) { return; }
				Script->OnUpdate(deltaTime);
			}
		}

	private:
		std::shared_ptr<ScriptableObject> Script = nullptr;
	};

	struct JinxScriptComponent
	{
		bool Enabled = true;

		JinxScriptComponent(Ref<Actor> actor, Jinx::RuntimePtr runtime, std::string& filePath)
		{
			Script = std::make_shared<JinxScript>(actor, runtime, filePath);
		}

		void OnUpdate(DeltaTime deltaTime)
		{
			if (Script != nullptr)
			{
				if (Script->Enabled == false) { return; }
				Script->OnUpdate(deltaTime);
			}
		}

	private:
		Ref<JinxScript> Script = nullptr;

	};

}