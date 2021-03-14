#pragma once
#include "Core/Core.h"
#include "Renderer/Material.h"
#include "Renderer/Texture.h"

#include <string>
#include <optional>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>

namespace SmolEngine
{
	enum class MaterialTexture : uint32_t
	{
		Albedro,
		Normal,
		Metallic,
		Roughness,
		AO
	};

	struct MaterialCreateInfo
	{				             
		float                                             Metallic = 1.0f;
		float                                             Albedro = 1.0f;
		float                                             Roughness = 1.0f;
		float                                             Specular = 9.0f;
					                                      
		std::string                                       Name = "";
		std::unordered_map<MaterialTexture, std::string>  Textures;

	private:

		friend class MaterialLibrary;
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Metallic, Albedro, Roughness, Specular, Name, Textures);
		}
	};

	class MaterialLibrary
	{
	public:

		MaterialLibrary();

		~MaterialLibrary();

		// Add / Delete

		int32_t Add(MaterialCreateInfo* infoCI, const std::string& path = "");

		bool Delete(const std::string& name);

		void Reset();

		// Save / Load MaterialCreateInfo

		bool Load(std::string& filePath, MaterialCreateInfo& out_info);

		bool Save(std::string& filePath, MaterialCreateInfo& info);

		// Getters

		Material* GetMaterial(int32_t ID);

		Material* GetMaterial(std::string& path);

		int32_t GetMaterialID(std::string& path);

		int32_t GetMaterialID(size_t& hashed_path);

		std::vector<Material>& GetMaterials();

		void GetMaterialsPtr(void*& data, uint32_t& size);

		const std::vector<Ref<Texture>>& GetTextures() const;

		static MaterialLibrary* GetSinglenton();

	private:

		// Helpers

		int32_t AddTexture(const Ref<Texture>& texture);

	private:

		static MaterialLibrary*                     s_Instance;
		int32_t                                     m_MaterialIndex = 0;
		uint32_t                                    m_TextureIndex = 0;

		std::vector<Material>                       m_Materials;
		std::vector<Ref<Texture>>                   m_Textures;
		std::hash<std::string_view>                 m_Hash{};

		std::unordered_map<size_t, int32_t>         m_MaterialMap;
	};
}