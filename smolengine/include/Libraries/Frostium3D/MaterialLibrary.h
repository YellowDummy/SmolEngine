#pragma once
#include "Common/Core.h"
#include "Primitives/Texture.h"

#include "Utils/GLM.h"

#include <string>
#include <optional>
#include <unordered_map>
#include <vector>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	enum class MaterialTexture : uint32_t
	{
		Albedro,
		Normal,
		Metallic,
		Roughness,
		AO,
		Emissive,
	};

	struct PBRMaterial
	{
		alignas(16) glm::vec4 Albedro;

		float    Metalness;
		float    Roughness;
		float    EmissionStrength;
		uint32_t UseAlbedroTex;

		uint32_t UseNormalTex;
		uint32_t UseMetallicTex;
		uint32_t UseRoughnessTex;
		uint32_t UseAOTex;

		uint32_t UseEmissiveTex;
		uint32_t AlbedroTexIndex;
		uint32_t NormalTexIndex;
		uint32_t MetallicTexIndex;

		uint32_t RoughnessTexIndex;
		uint32_t AOTexIndex;
		uint32_t EmissiveTexIndex;
	private:

		uint32_t pad1;
	};

	struct MaterialCreateInfo
	{
		void         SetMetalness(float value);
		void         SetRoughness(float value);
		void         SetEmissionStrength(float value);
		void         SetAlbedro(const glm::vec3& color);
		void         SetTexture(MaterialTexture type, const std::string& filePath);
		void         GetTextures(std::unordered_map<MaterialTexture, std::string*>& out_hashmap);

	public:
		bool         Used = false;
		float        Metallness = 0.2f;
		float        Roughness = 1.0f;
		float        EmissionStrength = 1.0f;
		std::string  AlbedroPath;
		std::string  NormalPath;
		std::string  MetallnessPath;
		std::string  RoughnessPath;
		std::string  AOPath;
		std::string  EmissivePath;
		glm::vec3    AlbedroColor = glm::vec3(1.0f);
	private:
		friend class MaterialLibrary;

	private:

		friend class MaterialLibrary;
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Metallness, Roughness, EmissionStrength, AlbedroPath, NormalPath, MetallnessPath, RoughnessPath, AOPath,
				EmissivePath, AlbedroColor.r, AlbedroColor.g, AlbedroColor.b);
		}
	};

	class MaterialLibrary
	{
	public:

		MaterialLibrary();
		~MaterialLibrary();

		uint32_t                  Add(MaterialCreateInfo* infoCI, const std::string& name);
		bool                      Delete(const std::string& name);
		void                      Reset();
		bool                      Load(const std::string& filePath, MaterialCreateInfo& out_info);
		bool                      Save(const std::string& filePath, MaterialCreateInfo& info);

		// Getters
		static MaterialLibrary*   GetSinglenton();
		PBRMaterial*              GetMaterial(uint32_t ID);
		PBRMaterial*              GetMaterial(std::string& path);
		int32_t                   GetMaterialID(std::string& path);
		int32_t                   GetMaterialID(size_t& hashed_path);
		std::vector<PBRMaterial>& GetMaterials();
		void                      GetMaterialsPtr(void*& data, uint32_t& size);
		void                      GetTextures(std::vector<Texture*>& out_textures) const;

	private:

		// Helpers
		uint32_t AddTexture(const std::string& path, uint32_t& useTetxure);

	private:
		static MaterialLibrary*               s_Instance;
		uint32_t                              m_MaterialIndex = 0;
		uint32_t                              m_TextureIndex = 0;
		std::unordered_map<size_t, uint32_t>  m_MaterialMap;
		std::vector<PBRMaterial>              m_Materials;
		std::vector<Texture*>                 m_Textures;
		std::hash<std::string_view>           m_Hash{};
	};
}