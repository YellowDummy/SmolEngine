#pragma once

#include "Core/Time.h"
#include "Core/Core.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Camera.h"
#include "Core/Application.h"
#include "Core/ECS/ScriptableObject.h"
#include "Core/Scripting/Jinx.h"
#include "Core/Renderer/Light2D.h"
#include "Core/Physics2D/Box2D/Body2D.h"
#include "Core/Animation/Animation2D.h"
#include "Core/Animation/Animation2DController.h"
#include "Core/Audio/AudioSource.h"
#include "Core/Audio/AudioClip.h"
#include "Core/UI/UICanvas.h"

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

		Ref<B2Data> B2Data = nullptr; // Should never be initialized outside of Rigidbody2D!

	private:

		friend class cereal::access;
		friend class Body2D;
		friend class Scene;
		friend class EditorLayer;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Rotation, Scale.x, Scale.y, WorldPos.x, WorldPos.y, WorldPos.z, ID, Enabled);
		}
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

		glm::vec4 Color = glm::vec4(1.0f);
		Ref<Texture2D> Texture = nullptr;
		std::string TexturePath;
		std::string FileName;

	private:

		friend class EditorLayer;
		friend class Scene;
		friend class cereal::access;


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
			archive(Enabled, aspectRatio, isSelected, Camera);
		}
	};


	struct Rigidbody2DComponent: public BaseComponent
	{
		Rigidbody2DComponent() = default;

		Rigidbody2DComponent(Ref<Actor> actor, Body2DType type);

		~Rigidbody2DComponent() = default;

		/// Forces

		void AddForce(const glm::vec2& force)
		{
			Body->GetBody()->ApplyForceToCenter({ force.x, force.y }, true);
		}

		void AddForce(const glm::vec2& force, const glm::vec2& point)
		{
			Body->GetBody()->ApplyForce({ force.x, force.y }, { point.x, point.y }, true);
		}

		/// Setters
		
		void SetAwake(bool value)
		{
			Body->GetBody()->SetAwake(value);
		}

		void SetGravity(float value)
		{
			Body->GetBody()->SetGravityScale(value);
		}

		/// Getters

		Body2DType GetType()
		{
			return (Body2DType)Body->m_Type;
		}

		/// RayCasting

		const RayCast2DHitInfo RayCast(const glm::vec2& startPoisition, const glm::vec2& targerPosition)
		{
			if (!Body->m_World)
			{
				RayCast2DHitInfo dummy;
				return dummy;
			}

			return Body->RayCast(startPoisition, targerPosition);
		}

		const std::vector<RayCast2DHitInfo> CircleCast(const glm::vec2& startPoisition, float distance)
		{
			if (!Body->m_World)
			{
				std::vector<RayCast2DHitInfo> dummy;
				return dummy;
			}

			return Body->CircleCast(startPoisition, distance);
		}

		/// Joints

		bool BindJoint(Rigidbody2DComponent& body, JointType type, JointInfo* info)
		{
			if (!body.IsValid())
			{
				return false;
			}

			return Body->BindJoint(body.Body.get(), type, info);
		}

		/// Checks

		bool IsValid() { return Body != nullptr; }

	private:

		Ref<Body2D> Body = nullptr;

		size_t ActorID = 0;

		bool ShowShape = true;

	private:

		friend class EditorLayer;
		friend class Scene;
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive) 
		{
			archive(Body, ActorID, Enabled, ShowShape);
		}
	};

	struct Animation2DControllerComponent : public BaseComponent
	{
		Animation2DControllerComponent() = default;

		Ref<Animation2DController> GetController() { return AnimationController; }

	private:

		friend class EditorLayer;
		friend class Scene;
		friend class cereal::access;

		Ref<Animation2DController> AnimationController = nullptr;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Enabled, ID, AnimationController);
		}
	};

	struct AudioSourceComponent: public BaseComponent
	{
		AudioSourceComponent() = default;

	private:

		Ref<AudioSource> AS = nullptr;

	private:

		friend class EditorLayer;
		friend class Scene;
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Enabled, ID, AS);
		}

	};

	struct CanvasComponent : public BaseComponent
	{
		CanvasComponent() = default;

		Ref<UIElement> GetElement(size_t index);

		Ref<UIButton> GetButton(size_t index);

		Ref<UITextLabel> GetTextLabel(size_t index);

		bool IsValid() { return Canvas != nullptr; }

	private:

		Ref<UICanvas> Canvas = nullptr;

	private:

		friend class EditorLayer;
		friend class Scene;
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Enabled, ID, Canvas);
		}

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

	private:

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
		ScriptObject(size_t sceneId, size_t actorID, const std::string& key, Ref<ScriptableObject> script)
			:SceneID(sceneId), ActorID(actorID), Script(script), keyName(key)
		{
		}

	private:

		void OnUpdate(DeltaTime deltaTime);

		void Start();

		void OnDestroy();

		void OnCollisionContact(Actor* actor);

		void OnCollisionExit(Actor* actor);

		void OnTriggerContact(Actor* actor);

		void OnTriggerExit(Actor* actor);



		size_t ActorID = 0;
		size_t SceneID = 0;

		std::string keyName = std::string("");

		std::shared_ptr<ScriptableObject> Script = nullptr;

	private:

		friend class EditorLayer;
		friend class Scene;
		friend class cereal::access;
		friend class CollisionListener2D;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Enabled, ActorID, SceneID, keyName);
		}

	};

}