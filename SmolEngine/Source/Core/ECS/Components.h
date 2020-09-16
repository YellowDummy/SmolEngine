#pragma once

#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Camera.h"
#include "Core/Application.h"
#include "Core/Time.h"
#include "Core/Core.h"

#include "Core/ECS/ScriptableObject.h"
#include "Core/Scripting/Jinx.h"

#include "Core/Physics2D/Rigidbody2D.h"
#include "Core/Renderer/Light2D.h"

#include <Jinx.hpp>
#include <box2d/box2d.h>

namespace SmolEngine
{
	class Actor;

	//TEMP
	struct B2Data
	{
		float* B2Rotation = nullptr;
		b2Transform* B2Pos = nullptr;
		b2Body* B2Body = nullptr;
	};

	struct BaseComponent
	{
		BaseComponent() = default;
		virtual ~BaseComponent() = default;

		bool Enabled = true;
		size_t ID = 0;
	};

	struct TransformComponent: public BaseComponent
	{
		glm::vec3 WorldPos = glm::vec3(0.0f);
		glm::vec2 Scale = glm::vec2(1.0f);;
		float Rotation = 0;

		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& worldPos = glm::vec3(1.0f), const glm::vec2& scale = glm::vec2(1.0f), const float rotation = 0)
			:WorldPos(worldPos), Scale(scale), Rotation(rotation) {}

		void SetTranform(float x, float y, float z = 1)
		{
			WorldPos.x = x; WorldPos.y = y; WorldPos.z = z;
		}

		void operator=(const TransformComponent& other);


	private:
		friend class cereal::access;
		friend class Rigidbody2D;
		friend class Scene;
		friend class EditorLayer;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Rotation, Scale.x, Scale.y, WorldPos.x, WorldPos.y, WorldPos.z, ID, Enabled);
		}

	private:
		Ref<B2Data> B2Data = nullptr; // Should never be initialized outside of Rigidbody2D!
	};

	struct Light2DComponent: public BaseComponent
	{
		Light2DComponent()
		{
			Light = std::make_shared<Light2D>();
		}

		Ref<Light2D> Light;

	private:
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::defer(Light), Enabled, ID);
			archive.serializeDeferments();
		}

	};

	struct Texture2DComponent: public BaseComponent
	{

		Texture2DComponent() = default;
		Texture2DComponent(const Texture2DComponent&) = default;
		Texture2DComponent(const std::string& filePath, const glm::vec4& color = glm::vec4(1.0f))
			:Color(color), TexturePath(filePath)
		{
			Texture = Texture2D::Create(filePath);
		}

	private:
		friend class EditorLayer;
		friend class Scene;
		friend class cereal::access;

		glm::vec4 Color = glm::vec4(1.0f);
		Ref<Texture2D> Texture = nullptr;
		std::string TexturePath;
		std::string FileName;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Enabled, Color.r, Color.g, Color.b, Color.a, TexturePath, FileName);
		}
	};

	struct CameraComponent: public BaseComponent
	{
		float aspectRatio = 0;
		bool isSelected = true;
		Ref<CameraController> Camera = nullptr;

		CameraComponent()
		{
			aspectRatio = (float)Application::GetApplication().GetWindowHeight() / (float)Application::GetApplication().GetWindowWidth();
			Camera = std::make_shared<CameraController>(aspectRatio);
			Camera->SetZoom(4.0f);
		}

	private:
		friend class EditorLayer;
		friend class Scene;
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Enabled, aspectRatio, isSelected);
		}
	};

	struct Rigidbody2DComponent: public BaseComponent
	{
		Rigidbody2DComponent() = default;
		~Rigidbody2DComponent() = default;

		Rigidbody2DComponent(Ref<Actor> actor, BodyType type)
			:Rigidbody(std::make_shared<Rigidbody2D>(actor, type)) {}

		void AddForce(const glm::vec2& force)
		{
			Rigidbody->GetBody()->ApplyForceToCenter({ force.x, force.y }, true);
		}

		void AddForce(const glm::vec2& force, const glm::vec2& point)
		{
			Rigidbody->GetBody()->ApplyForce({ force.x, force.y }, { point.x, point.y }, true);
		}

	private:
		friend class EditorLayer;
		friend class Scene;
		friend class cereal::access;

		bool ShowShape = true;
		Ref<Rigidbody2D> Rigidbody = nullptr;

		template<typename Archive>
		void serialize(Archive& archive) 
		{
			archive(cereal::defer(Rigidbody), Enabled, ShowShape);
			archive.serializeDeferments();
		}
	};


	struct Animation2DComponent : public BaseComponent
	{
		Animation2DComponent() = default;


	};

	struct JinxScriptComponent: public BaseComponent
	{
		JinxScriptComponent() = default;

		JinxScriptComponent(Ref<Actor> actor, Jinx::RuntimePtr runtime, std::string& filePath)
		{
			Script = std::make_shared<JinxScript>(actor, runtime, filePath);
		}

		void OnUpdate(DeltaTime deltaTime)
		{
		}

	private:
		Ref<JinxScript> Script = nullptr;
		std::string FilePath;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Enabled, FilePath);
		}

	};

	//TODO: Move to a separate header
	//An object is a component that should not be attached directly by the user (via script)

	//---------------------------------------------------------------------OBJECTS-----------------------------------------------------------------------//

	struct ScriptObject : public BaseComponent
	{
		ScriptObject() = default;
		ScriptObject(size_t sceneId, size_t actorID, std::string& key, Ref<ScriptableObject> script)
			:SceneID(sceneId), ActorID(actorID), Script(script), keyName(key)
		{
		}

		void OnUpdate(DeltaTime deltaTime);
		void Start();
		void OnDestroy();

	private:
		friend class EditorLayer;
		friend class Scene;
		friend class cereal::access;

		size_t ActorID = 0;
		size_t SceneID = 0;

		std::string keyName = std::string("");

		std::shared_ptr<ScriptableObject> Script = nullptr;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Enabled, ActorID, SceneID, keyName);
		}

	};

}