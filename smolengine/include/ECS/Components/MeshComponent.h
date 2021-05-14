#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#include <Frostium3D/Common/Mesh.h>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	struct MeshComponent: public BaseComponent
	{
		MeshComponent() = default;
		MeshComponent(uint32_t id)
			:BaseComponent(id) {}

		bool                                 bIsStatic = false;
		bool                                 bShow = true;			             
		int                                  ShadowType = 2;	                 
		Ref<Frostium::Mesh>                  Mesh = nullptr;
		std::string                          ModelPath = "";
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

		std::vector<MaterialData>            MaterialsData;

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(MaterialsData, bIsStatic, bShow,  ShadowType, ModelPath, ComponentID);
		}
	};
}