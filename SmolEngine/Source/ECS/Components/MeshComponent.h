#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"
#include "Renderer/Mesh.h"

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace SmolEngine
{
	struct MeshComponent: public BaseComponent
	{
		MeshComponent() = default;

		MeshComponent(uint32_t id)
			:BaseComponent(id) {}

		// Data

		bool                       bCastShadows = true;
		bool                       bIsStatic = false;
		bool                       bShow = true;

		int                        ShadowType = 2;
							       
		Ref<Mesh>                  Mesh = nullptr;
		std::vector<std::string>   MaterialNames;

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(bCastShadows, bIsStatic, ShadowType, MaterialNames, ComponentID);
		}
	};
}