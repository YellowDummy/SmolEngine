#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#ifndef FROSTIUM_SMOLENGINE_IMPL
#define FROSTIUM_SMOLENGINE_IMPL
#endif
#include <Frostium3D/Primitives/Mesh.h>

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	struct MeshComponent: public BaseComponent
	{
		MeshComponent() = default;
		MeshComponent(uint32_t id)
			:BaseComponent(id) {}

		struct MaterialData
		{
			std::string                      Path = "";
			uint32_t                         ID = 0;

			template<typename Archive>
			void serialize(Archive& archive)
			{
				archive(Path);
			}

		};

		enum class DefaultMeshType : uint32_t
		{
			None,
			Cube,
			Sphere,
			Capsule,
			Torus
		};

		bool                                 bIsStatic = false;
		bool                                 bShow = true;			                              
		Ref<Mesh>                            MeshPtr = nullptr;
		Mesh*                                DefaulPtr = nullptr;
		DefaultMeshType                      eDefaultType = DefaultMeshType::None;
		std::string                          ModelPath = "";			
		std::vector<MaterialData>            MaterialsData;

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(MaterialsData, eDefaultType, bIsStatic, bShow, ModelPath, ComponentID);
		}
	};
}