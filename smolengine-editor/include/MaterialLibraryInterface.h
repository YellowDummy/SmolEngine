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

	class MaterialLibraryInterface
	{
	public:

		MaterialLibraryInterface(TexturesLoader* loader);
		~MaterialLibraryInterface();

		void OpenExisting(const std::string& path);
		void OpenNew(const std::string& path);
		void Close();
		void Update();
		void Save();

		std::string GetCurrentPath() const;

	private:

		struct UniformBuffer
		{
			PBRMaterial material = {};
			glm::vec3   camPos = glm::vec3(0);
		};

		void InitPreviewRenderer();
		void LoadTexture(const std::string& filePath, MaterialTexture type, UniformBuffer& ubo, std::vector<Ref<Texture>>& textures);
		bool DrawTextureInfo(const char* header, std::string& outString, const std::string& title);
		void RenderImage();
		void ResetMaterial();
		void ApplyChanges();

	private:
		bool                                 m_bShowPreview = false;
		int                                  m_GeometryType = 1;
		const uint32_t                       m_BindingPoint = 277;
		std::future<void>                    m_DrawResult = {};
		std::string                          m_CurrentFilePath = "";
		TexturesLoader*                      m_TexturesLoader = nullptr;
		MaterialCreateInfo*                  m_MaterialCI = nullptr;
		PreviewRenderingData*                m_Data = nullptr;
		UniformBuffer*                       m_UBO = nullptr;
	};
}