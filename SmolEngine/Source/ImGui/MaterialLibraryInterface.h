#pragma once
#include "Core/Core.h"
#include "Renderer/MaterialLibrary.h"

namespace SmolEngine
{
	class GraphicsPipeline;
	class Framebuffer;
	class Mesh;
	class EditorCamera;
	class Texture;

	struct PreviewRenderingData
	{
		Ref<GraphicsPipeline>   Pipeline = nullptr;
		Ref<Mesh>               Mesh = nullptr;
		Ref<Framebuffer>        Framebuffer = nullptr;
		Ref<EditorCamera>       Camera = nullptr;

		glm::mat4               ViewProj = glm::mat4(1.0f);
		glm::vec3               CameraPos = glm::vec3(0, 0, 6);

		Ref<GraphicsPipeline>   TestPipeline = nullptr;
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

		MaterialLibraryInterface();

		void Draw(bool& show);

	private:

		void Reset();

		void InitPreviewRenderer();

		void DrawTextureInfo(const char* header, std::string& outString, std::string& dummy);

	private:
		
		bool                  m_bShowPreview = false;

		Ref<Texture>          m_Texture = nullptr;
		MaterialCreateInfo    m_MaterialCI = {};
		PreviewRenderingData  m_PreviewRenderingData = {};
		MaterialLibraryBuffer m_Buffer = {};
	};
}