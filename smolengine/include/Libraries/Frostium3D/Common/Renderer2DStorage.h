#pragma once

#include "GraphicsPipeline.h"

#include "Common/Framebuffer.h"
#include "Common/Text.h"
#include "Common/Mesh.h"
#include "Common/RendererShared.h"

namespace Frostium
{
	class Texture;

	struct Instance
	{
		glm::vec3* Position = nullptr;
		glm::vec3* Rotation = nullptr;
		glm::vec3* Scale = nullptr;
		glm::vec4* Color = nullptr;
		uint32_t* Layer = nullptr;
		uint32_t     TextureIndex = 0;
	};

	struct CmdBuffer
	{
		bool         OffsetApplied = false;
		uint32_t     Instances = 0;
		uint32_t     DataOffset = 0;

		void Reset()
		{
			DataOffset = 0;
			Instances = 0;
			OffsetApplied = 0;
		}
	};

	struct ShaderInstance
	{
		glm::mat4  Model;
		glm::vec4  Color;
		glm::ivec4 Params; // x - texture index
	};

	struct TextBuffer
	{
		glm::mat4    Model;
		Text* Obj = nullptr;
		uint32_t     TextureIndex = 0;
	};

	struct Renderer2DStorage
	{
		Renderer2DStorage()
		{
			Textures.resize(MaxTextureSlot);
			FontTextures.resize(MaxTextMessages);
		}

		~Renderer2DStorage() = default;

		// Limits
		static const uint32_t     Light2DBufferMaxSize = 100;
		static const uint32_t     MaxQuads = 15000;
		static const uint32_t     MaxTextMessages = 100;
		static const uint32_t     MaxLayers = 12;
		static const uint32_t     MaxTextureSlot = 4096;
		// Count tracking
		uint32_t                  InstIndex = 0;
		uint32_t                  TextIndex = 1;
		uint32_t                  SampleIndex = 1; // 0 - white dummy texture
		uint32_t                  FontSampleIndex = 0;
		// Framebuffers
		Framebuffer* MainFB = nullptr;
		// Pipelines
		GraphicsPipeline          MainPipeline = {};
		GraphicsPipeline          TextPipeline = {};
		// Refs
		Frustum* Frustum = nullptr;
		Texture* WhiteTetxure = nullptr;
		// Meshes
		Mesh                      PlaneMesh = {};
		// UBO's and SSBO's
		SceneData* SceneData = nullptr;
		ShaderInstance            ShaderInstances[MaxQuads];
		TextBuffer                TextMessages[MaxTextMessages];
		// Buffers
		Instance                  Instances[MaxQuads];
		CmdBuffer                 CommandBuffer[MaxLayers];
		std::vector<Texture*>     Textures;
		std::vector<Texture*>     FontTextures;
		// Bindings
		const uint32_t            InstancesBP = 1;
		const uint32_t            SamplersBP = 2;
		const uint32_t            SceneDataBP = 27;
		// Push Constants
		struct TextPC
		{
			glm::mat4             Model;
			glm::vec4             Color;
			uint32_t              TexIndex;
		};
		TextPC                    TextPushConstant;
		// Sizes
		const size_t              ShaderInstanceSize = sizeof(ShaderInstance);
		const size_t              TextPCSize = sizeof(TextPC);
	};
}