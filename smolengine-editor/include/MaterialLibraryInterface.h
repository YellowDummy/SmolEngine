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

	class MaterialLibraryInterface
	{
	public:

		MaterialLibraryInterface(EditorLayer* editor);
		~MaterialLibraryInterface();

		void OpenExisting(const std::string& path);
		void OpenNew(const std::string& path);
		void Close();
		void Update();
		void Save();

		std::string GetCurrentPath() const;

	private:

		void InitPreviewRenderer();
		void LoadTexture(const std::string& filePath, MaterialTexture type);
		bool DrawTextureInfo(const char* header, std::string& outString, const std::string& title);
		void RenderImage();
		void Reset();
		void ApplyChanges();

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
		std::string                          m_CurrentFilePath = "";
		Texture*                             m_RemoveTexture = nullptr;
		Texture*                             m_FolderTexture = nullptr;
		MaterialCreateInfo                   m_MaterialCI = {};
		PreviewRenderingData                 m_Data = {};
		UniformBuffer                        m_UBO{};
		std::vector<Ref<Texture>>            m_Textures;
	};
}