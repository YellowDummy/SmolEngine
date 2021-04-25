#pragma once
#include "Common/Core.h"
#include "Common/Material.h"
#include "Common/Texture.h"

#include <string>
#include <optional>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>

namespace Frostium
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
		// Setters
		void SetMetalness(float value);
		void SetRoughness(float value);
		void SetAlbedro(float value);
		void SetTexture(MaterialTexture type, const std::string& filePath);

	private:

		float                                             Metallic = 1.0f;
		float                                             Albedro = 1.0f;
		float                                             Roughness = 1.0f;
		bool                                              Used = false;
					                                      
		std::unordered_map<MaterialTexture, std::string>  Textures;

	private:

		friend class MaterialLibrary;
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Metallic, Albedro, Roughness, Textures);
		}
	};

	class MaterialLibrary
	{
	public:

		MaterialLibrary();
		~MaterialLibrary();

		int32_t Add(MaterialCreateInfo* infoCI, const std::string& path = "");
		bool Delete(const std::string& name);
		void Reset();

		// Serialization
		bool Load(const std::string& filePath, MaterialCreateInfo& out_info);
		bool Save(const std::string& filePath, MaterialCreateInfo& info);

		// Getters
		static MaterialLibrary* GetSinglenton();
		Material* GetMaterial(int32_t ID);
		Material* GetMaterial(std::string& path);
		int32_t GetMaterialID(std::string& path);
		int32_t GetMaterialID(size_t& hashed_path);
		std::vector<Material>& GetMaterials();
		void GetMaterialsPtr(void*& data, uint32_t& size);
		void GetTextures(std::vector<Texture*>& out_textures) const;

	private:

		// Helpers
		int32_t AddTexture(const std::string& path);
		std::string GetCompleteName(MaterialCreateInfo* infoCI);

	private:

		static MaterialLibrary*              s_Instance;
		int32_t                              m_MaterialIndex = 0;
		uint32_t                             m_TextureIndex = 0;

		std::vector<Material>                m_Materials;
		std::vector<Texture*>                m_Textures;
		std::hash<std::string_view>          m_Hash{};

		std::unordered_map<size_t, int32_t>  m_MaterialMap;
	};
}