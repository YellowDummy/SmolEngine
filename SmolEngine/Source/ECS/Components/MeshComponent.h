#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"
#include "ECS/Components/MaterialComponent.h"
#include "Renderer/Mesh.h"

namespace SmolEngine
{
	struct MeshComponent: public BaseComponent
	{
		MeshComponent() = default;

		MeshComponent(uint32_t id)
			:BaseComponent(id) {}

		// Data

		MaterialComponent Material = {};
		Ref<Mesh> Mesh = nullptr;

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Material, ComponentID);
		}
	};
}