#include "stdafx.h"
#include "MaterialLibrary.h"
#include "Core/AssetManager.h"
#include "ImGui/EditorConsole.h"

namespace SmolEngine
{
	MaterialLibrary* MaterialLibrary::s_Instance = new MaterialLibrary();

	MaterialLibrary::MaterialLibrary()
	{
		const uint32_t maxTextures = 4096;
		const uint32_t materials = 259;

		m_Textures.resize(maxTextures);
		m_Materials.reserve(materials);
		m_MaterialTable.reserve(materials);
	}

	MaterialLibrary::~MaterialLibrary()
	{

	}

	int32_t MaterialLibrary::Add(MaterialCreateInfo* infoCI)
	{
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
			newMaterial.m_MaterialProperties.PBRValues.x = infoCI->Metallic;
			newMaterial.m_MaterialProperties.PBRValues.y = infoCI->Roughness;
			newMaterial.m_MaterialProperties.PBRValues.z = infoCI->Albedro;
			newMaterial.m_MaterialProperties.PBRValues.w = infoCI->Specular;

			newMaterial.m_MaterialProperties.Indexes_1.x = AddTexture(Texture::Create(infoCI->Textures[MaterialTexture::Albedro]));
			newMaterial.m_MaterialProperties.Indexes_1.y = AddTexture(Texture::Create(infoCI->Textures[MaterialTexture::Normal]));
			newMaterial.m_MaterialProperties.Indexes_1.z = AddTexture(Texture::Create(infoCI->Textures[MaterialTexture::Metallic]));
			newMaterial.m_MaterialProperties.Indexes_1.w = AddTexture(Texture::Create(infoCI->Textures[MaterialTexture::Roughness]));
			newMaterial.m_MaterialProperties.Indexes_2.x = AddTexture(Texture::Create(infoCI->Textures[MaterialTexture::AO]));

			newMaterial.m_MaterialProperties.States_1.x = newMaterial.m_MaterialProperties.Indexes_1.x > -1 ? true : false;
			newMaterial.m_MaterialProperties.States_1.y = newMaterial.m_MaterialProperties.Indexes_1.y > -1 ? true : false;
			newMaterial.m_MaterialProperties.States_1.z = newMaterial.m_MaterialProperties.Indexes_1.z > -1 ? true : false;
			newMaterial.m_MaterialProperties.States_1.w = newMaterial.m_MaterialProperties.Indexes_1.w > -1 ? true : false;
			newMaterial.m_MaterialProperties.States_2.x = newMaterial.m_MaterialProperties.Indexes_2.x > -1 ? true : false;
		}

		materialID = m_MaterialIndex;
		m_Materials.emplace_back(newMaterial);
		m_MaterialTable[infoCI->Name] = materialID;
		m_MaterialIndex++;
		return materialID;
	}

	bool MaterialLibrary::Delete(const std::string& name)
	{
		return false;
	}

	void MaterialLibrary::Reset()
	{
		m_MaterialIndex = 0;
		m_TextureIndex = 0;

		m_Materials.clear();
		m_Textures.clear();
		m_MaterialTable.clear();
		m_Hasher.clear();
	}

	bool MaterialLibrary::Load(std::string& filePath, MaterialCreateInfo& out_info)
	{
		std::stringstream storage;
		std::ifstream file(filePath);
		if (!file)
		{
			NATIVE_ERROR("Could not open the file: {}", filePath);
			return false;
		}

		storage << file.rdbuf();
		{
			cereal::JSONInputArchive input{ storage };
			input(out_info.Metallic, out_info.Albedro,
				out_info.Roughness, out_info.Specular, out_info.Name,
				out_info.Textures);
		}

		return true;
	}

	bool MaterialLibrary::Save(std::string& filePath, MaterialCreateInfo& info)
	{
		std::stringstream storage;
		{
			cereal::JSONOutputArchive output{ storage };
			info.serialize(output);
		}

		std::ofstream myfile(filePath);
		if (myfile.is_open())
		{
			myfile << storage.str();
			myfile.close();
			return true;
		}

		return false;
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

	Material* MaterialLibrary::GetMaterial(uint32_t ID)
	{
		if (ID > m_MaterialIndex)
			return nullptr;

		return &m_Materials[ID];
	}

	Material* MaterialLibrary::GetMaterial(std::string& name)
	{
		auto& it = m_MaterialTable.find(name);
		if (it == m_MaterialTable.end())
			return nullptr;

		return &m_Materials[it->second];
	}

	std::optional<std::string> MaterialLibrary::GetMaterialName(int32_t id)
	{
		for (auto& [name, id] : m_MaterialTable)
		{
			if (id == id)
				return name;
		}

		return std::nullopt;
	}

	int32_t MaterialLibrary::GetMaterialID(std::string& name)
	{
		auto& it = m_MaterialTable.find(name);
		if (it == m_MaterialTable.end())
			return 0;

		return it->second;
	}

	std::vector<Material>& MaterialLibrary::GetMaterials()
	{
		return m_Materials;
	}

	const std::vector<Ref<Texture>>& MaterialLibrary::GetTextures() const
	{
		return m_Textures;
	}

	const std::unordered_map<std::string, uint32_t>& MaterialLibrary::GetMaterialTable() const
	{
		return m_MaterialTable;
	}

	void MaterialLibrary::GetMaterialsPtr(void*& data, uint32_t& size)
	{
		data = m_Materials.data();
		size = static_cast<uint32_t>(sizeof(Material) * m_Materials.size());
	}
}