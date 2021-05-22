#pragma once
#include "Core/Core.h"

#include <Frostium3D/MaterialLibrary.h>

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

	struct MaterialLibraryBuffer
	{
		std::string name = "";

		std::string albedro = "";
		std::string normal = "";
		std::string metallic = "";
		std::string roughness = "";
		std::string ao = "";
	};

	class MaterialLibraryInterface
	{
	public:

		MaterialLibraryInterface(EditorLayer* editor);
		~MaterialLibraryInterface();

		void Update(bool& show);

	private:

		void InitPreviewRenderer();
		void LoadTexture(const std::string& filePath, MaterialTexture type);
		void DrawTextureInfo(const char* header, std::string& outString, std::string& dummy);
		void RenderImage();
		void Reset();

	private:

		struct UniformBuffer
		{
			uint32_t  useAlbedro = 0;
			uint32_t  useNormal = 0;
			uint32_t  useRoughness = 0;
			uint32_t  useMetallic = 0;

			uint32_t  useAO = 0;
			glm::vec3 camPos = glm::vec3(0);
		};
		
		bool                                 m_bShowPreview = false;
		int                                  m_GeometryType = 1;
		const uint32_t                       m_BindingPoint = 277;
		Texture*                             m_RemoveTexture = nullptr;
		Texture*                             m_FolderTexture = nullptr;
		MaterialCreateInfo                   m_MaterialCI = {};
		PreviewRenderingData                 m_Data = {};
		MaterialLibraryBuffer                m_Buffer = {};
		UniformBuffer                        m_UBO{};
		std::vector<Ref<Texture>>            m_Textures;
	};
}