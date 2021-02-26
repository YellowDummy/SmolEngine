#pragma once
#include "Core/Core.h"
#include "Renderer/MaterialLibrary.h"

namespace SmolEngine
{
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

		void Draw(bool& show);

	private:

		void DrawTextureInfo(const char* header, std::string& outString, std::string& dummy);

	private:

		MaterialCreateInfo    m_MaterialCI = {};
		MaterialLibraryBuffer m_Buffer = {};
	};
}