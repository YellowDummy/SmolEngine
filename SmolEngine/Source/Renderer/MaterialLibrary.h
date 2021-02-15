#pragma once
#include "Core/Core.h"
#include "Renderer/Material.h"
#include "Renderer/Texture.h"
#include <string>

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
		float                    Metallic = 1.0f;
		float                    Albedro = 1.0f;
		float                    Roughness = 1.0f;
		float                    Specular = 9.0f;
					             
		std::string              Name = "";

		std::unordered_map<MaterialTexture,
			Ref<Texture>>        Textures;
	};

	class MaterialLibrary
	{
	public:

		int32_t Add(MaterialCreateInfo* infoCI);

		// Getters

		void GetMaterialsPtr(void*& data, uint32_t& size);

		Material* GetMaterial(uint32_t ID);

		std::vector<Material>& GetMaterials();

		const std::vector<Ref<Texture>>& GetTextures() const;

		static MaterialLibrary* GetSinglenton();

	private:

		int32_t AddTexture(const Ref<Texture>& texture);

		void Init();

	private:

		static MaterialLibrary*       s_Instance;
		bool                          m_Initialized = false;
		uint32_t                      m_MaterialIndex = 0;
		uint32_t                      m_TextureIndex = 0;

		std::vector<Material>         m_Materials;
		std::vector<Ref<Texture>>     m_Textures;

		std::unordered_map<size_t,
			std::string>              m_Hasher;
	};
}