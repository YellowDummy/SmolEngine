#pragma once
#include "Core/Core.h"
#include "Renderer/Material.h"
#include "Renderer/Texture.h"

#include <string>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>

namespace SmolEngine
{
	enum class MaterialTexture : uint16_t
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

		// Add / Delete

		int32_t Add(MaterialCreateInfo* infoCI);

		bool Delete(const std::string& name);

		// Save / Load

		bool Load();

		bool Save();

		// Getters

		Material* GetMaterial(uint32_t ID);

		Material* GetMaterial(std::string& name);

		std::string GetMaterialName(int32_t id);

		int32_t GetMaterialID(std::string& name);

		std::vector<Material>& GetMaterials();

		void GetMaterialsPtr(void*& data, uint32_t& size);

		const std::vector<Ref<Texture>>& GetTextures() const;

		const std::unordered_map<std::string, uint32_t>& GetMaterialTable() const;

		static MaterialLibrary* GetSinglenton();

	private:

		// Helpers

		void Init();

		int32_t AddTexture(const Ref<Texture>& texture);

	private:

		static MaterialLibrary*                     s_Instance;
		bool                                        m_Initialized = false;
		uint32_t                                    m_MaterialIndex = 0;
		uint32_t                                    m_TextureIndex = 0;

		std::string                                 m_SavePath = "../Resources/Saves/MaterialLibrary.data";
		std::vector<Material>                       m_Materials;
		std::vector<Ref<Texture>>                   m_Textures;
		std::vector<MaterialCreateInfo>             m_SaveData;

		std::unordered_map<std::string, uint32_t>   m_MaterialTable;
		std::unordered_map<size_t, std::string>     m_Hasher;

	private:

		friend class cereal::access;
		friend class Application;
		friend class MaterialLibraryInterface;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_SaveData);
		}
	};
}