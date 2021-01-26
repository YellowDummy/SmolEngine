#pragma once

#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Texture.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderTypes.h"

namespace SmolEngine
{
	struct GraphicsPipelineShaderCreateInfo
	{
		std::unordered_map<ShaderType, std::string> FilePaths;
		std::vector<Ref<Texture>> Textures;
		std::string SingleFilePath = "";

		bool Optimize = false;
		bool UseSingleFile = false;
	};

	struct VertexBufferCreateInfo
	{
		size_t Stride = 0;
		uint32_t BuffersCount = 1;
		bool IsAllocateMemOnly = false;
		BufferLayout* BufferLayot = nullptr;

		std::vector<size_t> Sizes;
		std::vector<void*> Vertices;
	};

	struct IndexBufferCreateInfo
	{
		size_t BuffersCount = 1;

		std::vector<size_t> Sizes;
		std::vector<size_t> IndicesCounts;
		std::vector<uint32_t*> Indices;
	};
}