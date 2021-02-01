#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"
#include "Renderer/Mesh.h"

namespace SmolEngine
{
	struct PBRTexture
	{
		Ref<Texture> Texture = nullptr;
		std::string FilePath = "";

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(filePath);
		}
	};

	struct PBRMaterial
	{
		PBRTexture          AlbedoTexture;
		PBRTexture          MetallicTetxure;
		PBRTexture          RoughnessTetxure;
		PBRTexture          NormalTexture;

		float               Albedo = 0.1f;
		float               Metallic = 0.1f;
		float               Roughness = 0.1f;

	public:

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(filePath);
		}
	};

	struct MeshComponent: public BaseComponent
	{
		MeshComponent() = default;

		MeshComponent(uint32_t id)
			:BaseComponent(id) {}

		// Data

		PBRMaterial Material = {};
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