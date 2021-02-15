#include "stdafx.h"
#include "MaterialLibrary.h"

namespace SmolEngine
{
	MaterialLibrary* MaterialLibrary::s_Instance = new MaterialLibrary();

	int32_t MaterialLibrary::Add(MaterialCreateInfo* infoCI)
	{
		if (!m_Initialized)
			Init();

		int32_t materialID = -1;
		if (infoCI->Name == "")
			return materialID;

		std::hash<std::string> hasher;
		size_t hash = hasher(infoCI->Name);

		auto& pos = m_Hasher.find(hash);
		if (pos != m_Hasher.end())
			return materialID;

		Material newMaterial = {};
		{
			newMaterial.m_MaterialProperties.Metallic = infoCI->Metallic;
			newMaterial.m_MaterialProperties.Roughness = infoCI->Roughness;
			newMaterial.m_MaterialProperties.Albedo = infoCI->Albedro;
			newMaterial.m_MaterialProperties.Specular = infoCI->Specular;

			newMaterial.m_MaterialProperties.UseAlbedroMap = infoCI->Textures[MaterialTexture::Albedro] != nullptr ? true : false;
			newMaterial.m_MaterialProperties.UseNormalMap = infoCI->Textures[MaterialTexture::Normal] != nullptr ? true : false;
			newMaterial.m_MaterialProperties.UseMetallicMap = infoCI->Textures[MaterialTexture::Metallic] != nullptr ? true : false;
			newMaterial.m_MaterialProperties.UseRoughnessMap = infoCI->Textures[MaterialTexture::Roughness] != nullptr ? true : false;
			newMaterial.m_MaterialProperties.UseAOMap = infoCI->Textures[MaterialTexture::AO] != nullptr ? true : false;

			newMaterial.m_MaterialProperties.AlbedroMapIndex = AddTexture(infoCI->Textures[MaterialTexture::Albedro]);
			newMaterial.m_MaterialProperties.NormalMapIndex = AddTexture(infoCI->Textures[MaterialTexture::Normal]);
			newMaterial.m_MaterialProperties.MetallicMapIndex = AddTexture(infoCI->Textures[MaterialTexture::Metallic]);
			newMaterial.m_MaterialProperties.RoughnessMapIndex = AddTexture(infoCI->Textures[MaterialTexture::Roughness]);
			newMaterial.m_MaterialProperties.AOMapIndex = AddTexture(infoCI->Textures[MaterialTexture::AO]);
		}

		materialID = m_MaterialIndex;
		m_Materials.emplace_back(newMaterial);
		m_MaterialIndex++;

		return materialID;
	}

	MaterialLibrary* MaterialLibrary::GetSinglenton()
	{
		return s_Instance;
	}

	int32_t MaterialLibrary::AddTexture(const Ref<Texture>& texture)
	{
		int32_t index = -1;
		if (texture)
		{
			index = m_TextureIndex;
			m_Textures[index] = texture;

			m_TextureIndex++;
			return index;
		}

		return index;
	}

	void MaterialLibrary::Init()
	{
		const uint32_t maxTextures = 4096;

		m_Textures.resize(maxTextures);
		m_Initialized = true;
	}

	Material* MaterialLibrary::GetMaterial(uint32_t ID)
	{
		if (ID > m_MaterialIndex || !m_Initialized)
			return nullptr;

		return &m_Materials[ID];
	}

	std::vector<Material>& MaterialLibrary::GetMaterials()
	{
		return m_Materials;
	}

	const std::vector<Ref<Texture>>& MaterialLibrary::GetTextures() const
	{
		return m_Textures;
	}

	void MaterialLibrary::GetMaterialsPtr(void*& data, uint32_t& size)
	{
		data = m_Materials.data();
		size = static_cast<uint32_t>(sizeof(Material) * m_Materials.size());
	}
}