#pragma once
#include "TexturesLoader.h"

#include <Core/Core.h>
#include <Frostium3D/MaterialLibrary.h>
#include <future>

namespace SmolEngine
{
	class EditorLayer;
	class GraphicsPipeline;
	class Framebuffer;
	class Mesh;
	class EditorCamera;
	class Texture;

	struct PreviewRenderingData
	{
		Ref<GraphicsPipeline>   Pipeline = nullptr;
		Ref<Framebuffer>        Framebuffer = nullptr;
		Ref<EditorCamera>       Camera = nullptr;
	};

	class MaterialInspector
	{
	public:

		MaterialInspector();
		~MaterialInspector();

		void RenderMaterialIcon(Framebuffer* fb, MaterialCreateInfo* material);
		void OpenExisting(const std::string& path);
		void OpenNew(const std::string& path);
		void Close();
		void Update();
		void Save();

		std::string GetCurrentPath() const;
	private:

		void InitPreviewRenderer();
		void LoadTexture(TextureCreateInfo* info, MaterialTexture type, std::vector<Texture*>& textures);
		bool DrawTextureInfo(TextureCreateInfo& texInfo, const std::string& title);
		void RenderImage();
		void ResetMaterial();
		void ApplyChanges();

	private:
		bool                                          m_bShowPreview = false;
		int                                           m_GeometryType = 1;
		const uint32_t                                m_BindingPoint = 277;
		std::future<void>                             m_DrawResult = {};
		std::string                                   m_CurrentFilePath = "";
		std::string                                   m_CurrentName = "";
		PBRMaterial                                   m_Material = {};
		TexturesLoader*                               m_TexturesLoader = nullptr;
		MaterialCreateInfo*                           m_MaterialCI = nullptr;
		PreviewRenderingData*                         m_Data = nullptr;
		std::unordered_map<std::string, Ref<Texture>> m_Textures;
	};
}