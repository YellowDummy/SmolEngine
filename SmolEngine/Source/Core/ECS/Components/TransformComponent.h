#pragma once
#include "Core/Core.h"
#include "Core/ECS/Components/BaseComponent.h"

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>


namespace SmolEngine
{
	struct TransformComponent: public BaseComponent
	{
		TransformComponent();

		TransformComponent(uint32_t id)
			:BaseComponent(id) {}

		/// Operators

		void operator=(const TransformComponent & other);

		/// Data

		glm::vec3 WorldPos = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);;
		glm::vec3 Scale = glm::vec3(1.0f);;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class Body2D;
		friend class Scene;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Rotation.x, Rotation.y, Rotation.z, Scale.x, Scale.y, Scale.z, WorldPos.x, WorldPos.y, WorldPos.z, ComponentID);
		}
	};
}