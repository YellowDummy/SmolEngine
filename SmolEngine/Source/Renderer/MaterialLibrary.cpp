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
		newMaterial.m_MaterialProperties.Metallic = infoCI->Metallic;
		newMaterial.m_MaterialProperties.Roughness = infoCI->Roughness;
		newMaterial.m_MaterialProperties.Ambient = infoCI->Ambient;
		newMaterial.m_MaterialProperties.Specular = infoCI->Specular;

		materialID = m_MaterialsCount;
		m_Materials.emplace_back(newMaterial);
		m_MaterialsCount++;

		return materialID;
	}

	MaterialLibrary* MaterialLibrary::GetSinglenton()
	{
		return s_Instance;
	}

	void MaterialLibrary::Init()
	{
		m_Materials.reserve(m_ReserveSize);
		m_Initialized = true;
	}

	Material* MaterialLibrary::GetMaterial(uint32_t ID)
	{
		if (ID > m_MaterialsCount || !m_Initialized)
			return nullptr;

		return &m_Materials[ID];
	}

	std::vector<Material>& MaterialLibrary::GetMaterials()
	{
		return m_Materials;
	}

	void MaterialLibrary::GetMaterialsPtr(void*& data, uint32_t& size)
	{
		data = m_Materials.data();
		size = static_cast<uint32_t>(sizeof(Material) * m_Materials.size());
	}
}