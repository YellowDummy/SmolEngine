#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"
#include "Renderer/Mesh.h"

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	struct MeshComponent: public BaseComponent
	{
		MeshComponent() = default;

		MeshComponent(uint32_t id)
			:BaseComponent(id) {}

		// Data

		bool                                 bCastShadows = true;
		bool                                 bIsStatic = false;
		bool                                 bShow = true;
								             
		int                                  ShadowType = 2;
							                 
		Ref<Mesh>                            Mesh = nullptr;
		std::string                          FilePath = "";

		struct MeshData
		{
			std::string  MaterialName;
			int32_t      MaterialID; // runtime value, no need to serialize

			template<typename Archive>
			void serialize(Archive& archive)
			{
				archive(MaterialName);
			}
		};

		std::vector<MeshData>                MeshData;

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(MeshData, bCastShadows, bIsStatic, bShow,  ShadowType, FilePath, ComponentID);
		}
	};
}