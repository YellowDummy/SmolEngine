#include "stdafx.h"
#include "MaterialLibrary.h"

namespace SmolEngine
{
	MaterialLibrary* MaterialLibrary::s_Instance = new MaterialLibrary();

	int32_t MaterialLibrary::Add(MaterialCreateInfo* infoCI)
	{
		int32_t materialID = -1;
		std::hash<std::string> hasher;
		size_t hash = hasher(infoCI->Name);

		if (infoCI->Name == "")
			return materialID;

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

	Material* MaterialLibrary::GetMaterial(uint32_t ID)
	{
		if (ID > m_MaterialsCount)
			return nullptr;

		return &m_Materials[ID];
	}

	void MaterialLibrary::GetMaterialsPtr(void*& data, uint32_t& size)
	{
		data = m_Materials.data();
		size = static_cast<uint32_t>(sizeof(Material) * m_Materials.size());
	}
}