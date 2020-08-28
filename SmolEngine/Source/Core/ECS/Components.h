#pragma once

#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Camera.h"
#include "Core/Application.h"
#include "Core/Time.h"
#include "Core/Core.h"

#include "Core/ECS/ScriptableObject.h"
#include "Core/Scripting/Jinx.h"

#include "Core/Physics2D/Rigidbody2D.h"

#include <glm/glm.hpp>
#include <Jinx.hpp>
#include <box2d/box2d.h>

namespace SmolEngine
{
	class Actor;

	struct B2Data
	{
		float* B2Rotation = nullptr;
		b2Transform* B2Pos = nullptr;
		b2Body* B2Body = nullptr;
	};

	struct TransfromComponent
	{
		glm::vec3 WorldPos = glm::vec3(1.0f);
		glm::vec2 Scale = glm::vec2(1.0f);;
		float Rotation = 0;

		Ref<B2Data> B2Data = nullptr; // Should never be initialized outside of Rigidbody2D!

		TransfromComponent(const TransfromComponent&) = default;
		TransfromComponent(const glm::vec3& worldPos = glm::vec3(1.0f), const glm::vec2& scale = glm::vec2(1.0f), const float rotation = 0)
			:WorldPos(worldPos), Scale(scale), Rotation(rotation) {}

		void SetTranform(float x, float y, float z = 1)
		{
			WorldPos.x = x; WorldPos.y = y; WorldPos.z = z;
		}

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
		float aspectRatio = (float)Application::GetApplication().GetWindowHeight() / (float)Application::GetApplication().GetWindowWidth();
		Ref<CameraController> Camera = nullptr;

		CameraComponent()
		{
			Camera = std::make_shared<CameraController>(aspectRatio);
			Camera->SetZoom(4.0f);
		}

	private:
		friend class EditorLayer;
		friend class Scene;

		bool isSelected = true;
	};

	struct Rigidbody2DComponent
	{
		bool Enabled = true;
		Rigidbody2DComponent(Ref<Actor> actor, b2World* world, BodyType type)
			:Rigidbody(std::make_shared<Rigidbody2D>(actor, world, type)) {}

		void AddForce(const glm::vec2& force)
		{
			Rigidbody->GetBody()->ApplyForceToCenter({ force.x, force.y }, true);
		}

	private:
		friend class EditorLayer;
		friend class Scene;

		Ref<Rigidbody2D> Rigidbody;
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

		void OnUpdate(DeltaTime deltaTime);
		void Start();
		void OnDestroy();

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