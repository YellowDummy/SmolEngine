#pragma once
#include "Core/Core.h"
#include "Renderer/Material.h"

#include <string>

namespace SmolEngine
{
	struct MaterialCreateInfo
	{				             
		float                    Metallic = 0.6f;
		float                    Roughness = 0.6f;
		float                    Ambient = 1.0f;
		float                    Specular = 9.0f;
					             
		std::string              Name = "";
	};

	class MaterialLibrary
	{
	public:

		int32_t Add(MaterialCreateInfo* infoCI);

		// Getters

		static MaterialLibrary* GetSinglenton();

		Material* GetMaterial(uint32_t ID);

		std::vector<Material>& GetMaterials();

		void GetMaterialsPtr(void*& data, uint32_t& size);

	private:

		static MaterialLibrary*       s_Instance;

		uint32_t                      m_MaterialsCount = 0;
		std::vector<Material>         m_Materials;
		std::unordered_map<size_t,
			std::string>              m_Hasher;
	};
}