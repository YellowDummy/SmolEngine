#pragma once
#include "Core/Core.h"

#include <Frostium3D/MaterialLibrary.h>

class Frostium::GraphicsPipeline;
class Frostium::Framebuffer;
class Frostium::Mesh;
class Frostium::EditorCamera;
class Frostium::Texture;

namespace SmolEngine
{
	class EditorLayer;

	struct PreviewRenderingData
	{
		Ref<Frostium::GraphicsPipeline>   Pipeline = nullptr;
		Ref<Frostium::Framebuffer>        Framebuffer = nullptr;
		Ref<Frostium::EditorCamera>       Camera = nullptr;
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
		void LoadTexture(const std::string& filePath, Frostium::MaterialTexture type);
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
		Frostium::Texture*                   m_RemoveTexture = nullptr;
		Frostium::Texture*                   m_FolderTexture = nullptr;
		Frostium::MaterialCreateInfo         m_MaterialCI = {};
		PreviewRenderingData                 m_Data = {};
		MaterialLibraryBuffer                m_Buffer = {};
		UniformBuffer                        m_UBO{};
		std::vector< Ref<Frostium::Texture>> m_Textures;
	};
}