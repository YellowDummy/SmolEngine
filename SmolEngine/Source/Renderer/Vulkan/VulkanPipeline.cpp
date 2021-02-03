#include "stdafx.h"
#include "VulkanPipeline.h"

#include "Renderer/GraphicsPipeline.h"
#include "Renderer/Vulkan/VulkanRenderPass.h"
#include "Renderer/Vulkan/VulkanPipelineSpecification.h"
#include "Renderer/Vulkan/VulkanContext.h"

namespace SmolEngine
{
	VulkanPipeline::VulkanPipeline()
	{
		m_Device = VulkanContext::GetDevice().GetLogicalDevice();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		m_VulkanPipelineSpecification = {};
	}

	bool VulkanPipeline::Invalidate(VulkanPipelineSpecification& pipelineSpec)
	{
		if(!pipelineSpec.Device || !pipelineSpec.Shader || !pipelineSpec.TargetSwapchain || pipelineSpec.DescriptorSets == 0)
		{
			assert(false);
			return false;
		}

		BuildDescriptors(pipelineSpec.Shader, pipelineSpec.DescriptorSets);
		m_VulkanPipelineSpecification = pipelineSpec;
		m_VulkanPipelineSpecification.Initialized = true;

		pipelineSpec.IsTargetsSwapchain ? m_TargetRenderPass = VulkanRenderPass::GetVkRenderPassSwapchainLayout() :
			m_TargetRenderPass = VulkanRenderPass::GetVkRenderPassFramebufferLayout();

		if (pipelineSpec.IsUseMRT)
			m_TargetRenderPass = VulkanRenderPass::GetVkRenderPassDeferredLayout();

		m_SetLayout.clear();
		m_SetLayout.reserve(m_Descriptors.size());
		for (auto& descriptor : m_Descriptors)
		{
			m_SetLayout.push_back(descriptor.m_DescriptorSetLayout);
		}

		VkPipelineLayoutCreateInfo pipelineLayoutCI = {};
		{
			pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCI.pNext = nullptr;
			pipelineLayoutCI.setLayoutCount = static_cast<uint32_t>(m_SetLayout.size());
			pipelineLayoutCI.pSetLayouts = m_SetLayout.data();
			pipelineLayoutCI.pushConstantRangeCount = static_cast<uint32_t>(pipelineSpec.Shader->m_VkPushConstantRanges.size());
			pipelineLayoutCI.pPushConstantRanges = pipelineSpec.Shader->m_VkPushConstantRanges.data();

			VK_CHECK_RESULT(vkCreatePipelineLayout(pipelineSpec.Device->GetLogicalDevice(), &pipelineLayoutCI, nullptr, &m_PipelineLayout));
		}

		m_FilePath = "../Resources/Cached/" + pipelineSpec.Name;
		return true;
	}

	bool VulkanPipeline::CreatePipeline(DrawMode mode)
	{
		const auto& shader = m_VulkanPipelineSpecification.Shader;
		const auto& swapchain = m_VulkanPipelineSpecification.TargetSwapchain;

		// Create the graphics pipeline
		// Vulkan uses the concept of rendering pipelines to encapsulate fixed states, replacing OpenGL's complex state machine
		// A pipeline is then stored and hashed on the GPU making pipeline changes very fast
		// Note: There are still a few dynamic states that are not directly part of the pipeline (but the info that they are used is)

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		{
			pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			// The layout used for this pipeline (can be shared among multiple pipelines using the same layout)
			pipelineCreateInfo.layout = m_PipelineLayout;
			// Renderpass this pipeline is attached to
			pipelineCreateInfo.renderPass = m_TargetRenderPass;
		}

		// Construct the different states making up the pipeline

		// Input assembly state describes how primitives are assembled
		// This pipeline will assemble vertex data as a triangle lists (though we only use one triangle)
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
		inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyState.topology = GetVkTopology(mode);

		// Rasterization state
		VkPipelineRasterizationStateCreateInfo rasterizationState = {};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.polygonMode = GetVkPolygonMode(mode);
		rasterizationState.cullMode = VK_CULL_MODE_NONE;
		rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationState.depthClampEnable = VK_TRUE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.depthBiasEnable = VK_FALSE;
		rasterizationState.lineWidth = 1.0f;

		// Color blend state describes how blend factors are calculated (if used)
		// We need one blend attachment state per color attachment (even if blending is not used)
		std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentState;
		if (m_VulkanPipelineSpecification.IsUseMRT)
		{
			blendAttachmentState.resize(4);

			blendAttachmentState[0].colorWriteMask = 0xf;
			blendAttachmentState[0].blendEnable = VK_FALSE;

			blendAttachmentState[1].colorWriteMask = 0xf;
			blendAttachmentState[1].blendEnable = VK_FALSE;

			blendAttachmentState[2].colorWriteMask = 0xf;
			blendAttachmentState[2].blendEnable = VK_FALSE;

			blendAttachmentState[3].colorWriteMask = 0xf;
			blendAttachmentState[3].blendEnable = VK_FALSE;
		}
		else
		{
			blendAttachmentState.resize(1);
			blendAttachmentState[0].colorWriteMask = 0xf;
			blendAttachmentState[0].blendEnable = VK_FALSE;
			if (m_VulkanPipelineSpecification.IsAlphaBlendingEnabled)
			{
				blendAttachmentState[0].blendEnable = VK_TRUE;
				blendAttachmentState[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				blendAttachmentState[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				blendAttachmentState[0].colorBlendOp = VK_BLEND_OP_ADD;
				blendAttachmentState[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				blendAttachmentState[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				blendAttachmentState[0].alphaBlendOp = VK_BLEND_OP_ADD;
			}
		}

		VkPipelineColorBlendStateCreateInfo colorBlendState = {};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.attachmentCount = static_cast<uint32_t>(blendAttachmentState.size());
		colorBlendState.pAttachments = blendAttachmentState.data();

		// Viewport state sets the number of viewports and scissor used in this pipeline
		// Note: This is actually overridden by the dynamic states (see below)
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Enable dynamic states
		// Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
		// To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in the command buffer.
		// For this example we will set the viewport and scissor using dynamic states
		std::vector<VkDynamicState> dynamicStateEnables;
		dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.pDynamicStates = dynamicStateEnables.data();
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

		// Depth and stencil state containing depth and stencil compare and test operations
		// We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
		VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = m_VulkanPipelineSpecification.IsDepthTestEnabled ? VK_TRUE : VK_FALSE;;
		depthStencilState.depthWriteEnable = VK_TRUE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
		depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
		depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.front = depthStencilState.back;

		// Multi sampling state
		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		if (m_VulkanPipelineSpecification.IsUseMRT)
		{
			multisampleState.rasterizationSamples = VulkanContext::GetDevice().GetMSAASamplesCount();
			multisampleState.sampleShadingEnable = VK_TRUE;
			multisampleState.minSampleShading = 0.2f;
			multisampleState.pSampleMask = nullptr;
		}

		uint32_t vertexInputAttributsCounts = 0;
		for (const auto& inputInfo : m_VulkanPipelineSpecification.VertexInputInfos)
		{
			for (const auto& element : inputInfo.Layout.GetElements())
			{
				vertexInputAttributsCounts++;
			}
		}

		std::vector<VkVertexInputBindingDescription> vertexInputBindings(m_VulkanPipelineSpecification.VertexInputInfos.size());
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributs(vertexInputAttributsCounts);
		{
			vertexInputAttributsCounts = 0;
			uint32_t index = 0;
			uint32_t location = 0;
			for (const auto& inputInfo : m_VulkanPipelineSpecification.VertexInputInfos)
			{
				// Vertex input binding
				// This example uses a single vertex input binding at binding point 0 (see vkCmdBindVertexBuffers)
				{
					vertexInputBindings[index].binding = index;
					vertexInputBindings[index].stride = inputInfo.Stride;
					vertexInputBindings[index].inputRate = inputInfo.IsInputRateInstance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
				}

				// Vertex input descriptions
				// Specifies the vertex input parameters for a pipeline
				{
					for (const auto& element : inputInfo.Layout.GetElements())
					{
						vertexInputAttributs[vertexInputAttributsCounts].binding = index;
						vertexInputAttributs[vertexInputAttributsCounts].location = location;
						vertexInputAttributs[vertexInputAttributsCounts].format = GetVkInputFormat(element.type);
						vertexInputAttributs[vertexInputAttributsCounts].offset = element.offset;

						location++;
						vertexInputAttributsCounts++;
					}
				}

				index++;
			}
		}

		// Vertex input state used for pipeline creation
		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		if (m_VulkanPipelineSpecification.VertexInputInfos.size() > 0)
		{
			vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
			vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
			vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributs.size());
			vertexInputState.pVertexAttributeDescriptions = vertexInputAttributs.data();
		}

		if (m_VulkanPipelineSpecification.IsUseMRT)
		{
			auto& stages = shader->GetVkPipelineShaderStages();
			for (auto& stage : stages)
			{
				if (stage.stage == VK_SHADER_STAGE_FRAGMENT_BIT)
				{
					// Use specialization constants to pass number of samples to the shader (used for MSAA resolve)
					VkSpecializationMapEntry specializationEntry{};
					specializationEntry.constantID = 0;
					specializationEntry.offset = 0;
					specializationEntry.size = sizeof(uint32_t);

					uint32_t specializationData = VulkanContext::GetDevice().GetMSAASamplesCount();

					VkSpecializationInfo specializationInfo;
					specializationInfo.mapEntryCount = 1;
					specializationInfo.pMapEntries = &specializationEntry;
					specializationInfo.dataSize = sizeof(specializationData);
					specializationInfo.pData = &specializationData;

					stage.pSpecializationInfo = &specializationInfo;
					break;
				}
			}
		}

		// Set pipeline shader stage info
		pipelineCreateInfo.stageCount = static_cast<uint32_t>(shader->GetVkPipelineShaderStages().size());
		pipelineCreateInfo.pStages = shader->GetVkPipelineShaderStages().data();

		// Assign the pipeline states to the pipeline creation info structure
		pipelineCreateInfo.pVertexInputState = &vertexInputState;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineCreateInfo.pRasterizationState = &rasterizationState;
		pipelineCreateInfo.pColorBlendState = &colorBlendState;
		pipelineCreateInfo.pMultisampleState = &multisampleState;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.pDepthStencilState = &depthStencilState;
		pipelineCreateInfo.pDynamicState = &dynamicState;

		std::string name = std::string(m_FilePath + "_pipeline_" + std::to_string(m_PipelineCaches.size()) + ".cached");
		bool is_loaded = CreateOrLoadCached(name, mode);

		// Create rendering pipeline using the specified states
		VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_Device, m_PipelineCaches[mode], 1, &pipelineCreateInfo, nullptr, &m_Pipelines[mode]));
		if (!is_loaded)
		{
			SaveCache(name, mode);
		}

		return true;
	}

	bool VulkanPipeline::ReCreate()
	{
		if (m_VulkanPipelineSpecification.Initialized)
		{
			Destroy();
			if (Invalidate(m_VulkanPipelineSpecification))
			{
				for (auto mode : m_VulkanPipelineSpecification.PipelineDrawModes)
				{
					CreatePipeline(mode);
				}
				return true;
			}

		}
		return false;
	}

	void VulkanPipeline::Destroy()
	{
		for (auto& [key, cache] : m_PipelineCaches)
		{
			vkDestroyPipelineCache(m_Device, cache, nullptr);
		}

		for (auto& [key, pipeline] : m_Pipelines)
		{
			vkDestroyPipeline(m_Device, pipeline, nullptr);
		}

		vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
		m_Pipelines.clear();
		m_PipelineCaches.clear();
	}

	bool VulkanPipeline::UpdateCubeMap(const VulkanTexture* cubeMap, uint32_t bindingPoint, uint32_t setIndex)
	{
		return m_Descriptors[setIndex].UpdateCubeMap(cubeMap, bindingPoint);
	}

	bool VulkanPipeline::UpdateSamplers2D(const std::vector<VulkanTexture*>& textures, uint32_t bindingPoint,  uint32_t setIndex)
	{
		return m_Descriptors[setIndex].Update2DSamplers(textures, bindingPoint);
	}

	bool VulkanPipeline::SaveCache(const std::string& fileName, DrawMode mode)
	{
		FILE* f = fopen(fileName.c_str(), "wb");
		if (f)
		{
			size_t size = 0;
			void* data = nullptr;
			const auto& device = m_VulkanPipelineSpecification.Device->GetLogicalDevice();

			vkGetPipelineCacheData(device, m_PipelineCaches[mode], &size, nullptr);
			data = (char*)malloc(sizeof(char) * size);
			vkGetPipelineCacheData(device, m_PipelineCaches[mode], &size, data);

			size_t result = fwrite(data, sizeof(char), size, f);
			if (result != size)
			{
				NATIVE_ERROR("VulkanPipeline::SaveCache(): cache was not saved");
				fclose(f);
				free(data);
				return false;
			}

			fclose(f);
			free(data);
			return true;
		}

		return false;
	}

	bool VulkanPipeline::CreateOrLoadCached(const std::string& fileName, DrawMode mode)
	{
		VkPipelineCache cache = nullptr;

		FILE* f = fopen(fileName.c_str(), "rb");
		if (f)
		{
			fseek(f, 0, SEEK_END);
			size_t size = ftell(f);
			rewind(f);
			if (size != 0)
			{
				void* data = (char*)malloc(sizeof(char) * size);
				fread(data, 1, size, f);
				fclose(f);

				VkPipelineCacheCreateInfo pipelineCacheCI = {};
				{
					pipelineCacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
					pipelineCacheCI.initialDataSize = size;
					pipelineCacheCI.pInitialData = data;
				}

				VK_CHECK_RESULT(vkCreatePipelineCache(m_Device, &pipelineCacheCI, nullptr, &cache));
				m_PipelineCaches[mode] = cache;
				free(data);
				return true;
			}
		}

		VkPipelineCacheCreateInfo pipelineCacheCI = {};
		{
			pipelineCacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		}

		VK_CHECK_RESULT(vkCreatePipelineCache(m_Device, &pipelineCacheCI, nullptr, &cache));
		m_PipelineCaches[mode] = cache;
		return false;
	}

	const VkPipeline& VulkanPipeline::GetVkPipeline(DrawMode mode)
	{
		return m_Pipelines[mode];
	}

	const VkPipelineLayout& VulkanPipeline::GetVkPipelineLayot() const
	{
		return m_PipelineLayout;
	}

	const VkDescriptorSet VulkanPipeline::GetVkDescriptorSets(uint32_t setIndex) const
	{
		return m_Descriptors[setIndex].GetDescriptorSets();
	}

	void VulkanPipeline::BuildDescriptors(VulkanShader* shader, uint32_t DescriptorSets)
	{
		m_Descriptors.clear();
		if (m_DescriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
		}

		std::vector< VkDescriptorPoolSize> DescriptorPoolSizes(2);
		uint32_t unformsBufferSize = 1;
		uint32_t resourcesBufferSize = 1;

		if (shader->m_UniformBuffers.size() > 0)
			unformsBufferSize = static_cast<uint32_t>(shader->m_UniformBuffers.size());

		if (shader->m_UniformResources.size() > 0)
			resourcesBufferSize = static_cast<uint32_t>(shader->m_UniformResources.size());

		// UBO
		{
			VkDescriptorPoolSize tempPoolSize = {};
			{
				tempPoolSize.descriptorCount = unformsBufferSize;
				tempPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

			}

			DescriptorPoolSizes[0] = tempPoolSize;
		}

		// Samplers
		{
			VkDescriptorPoolSize tempPoolSize = {};
			{
				tempPoolSize.descriptorCount = resourcesBufferSize;
				tempPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			}

			DescriptorPoolSizes[1] = tempPoolSize;
		}

		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		{
			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.pNext = nullptr;
			descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(DescriptorPoolSizes.size());
			descriptorPoolInfo.pPoolSizes = DescriptorPoolSizes.data();
			descriptorPoolInfo.maxSets = DescriptorSets;

			VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device, &descriptorPoolInfo, nullptr, &m_DescriptorPool));
		}

		m_Descriptors.resize(DescriptorSets);
		for (uint32_t i = 0; i < DescriptorSets; ++i)
		{
			m_Descriptors[i].GenDescriptorSet(shader, m_DescriptorPool);
			m_Descriptors[i].GenUniformBuffersDescriptors(shader);
			m_Descriptors[i].GenSamplersDescriptors(shader);

		}
	}

	//TODO: add more formats
	VkFormat VulkanPipeline::GetVkInputFormat(ShaderDataType type)
	{
		switch (type)
		{
		case SmolEngine::ShaderDataType::None:
			break;
		case SmolEngine::ShaderDataType::Float:
			return VK_FORMAT_R32_SFLOAT;
		case SmolEngine::ShaderDataType::Float2:
			return VK_FORMAT_R32G32_SFLOAT;
		case SmolEngine::ShaderDataType::Float3:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case SmolEngine::ShaderDataType::Float4:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case SmolEngine::ShaderDataType::Mat3:
			break;
		case SmolEngine::ShaderDataType::Mat4:
			break;
		case SmolEngine::ShaderDataType::Int:
			return VK_FORMAT_R32_SINT;
		case SmolEngine::ShaderDataType::Int2:
			return VK_FORMAT_R32G32_SINT;
		case SmolEngine::ShaderDataType::Int3:
			return VK_FORMAT_R32G32B32_SINT;
		case SmolEngine::ShaderDataType::Int4:
			return VK_FORMAT_R32G32B32A32_SINT;
		case SmolEngine::ShaderDataType::Bool:
			return VK_FORMAT_R32_SINT;
		default:
			break;
		}

		//temp
		return VK_FORMAT_R32G32B32_SFLOAT;
	}

	VkPrimitiveTopology VulkanPipeline::GetVkTopology(DrawMode mode)
	{
		switch (mode)
		{
		case SmolEngine::DrawMode::Triangle:
			return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case SmolEngine::DrawMode::Line:
			return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case SmolEngine::DrawMode::Fan:
			return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
		default:
			return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
	}

	VkPolygonMode VulkanPipeline::GetVkPolygonMode(DrawMode mode)
	{
		switch (mode)
		{
		case SmolEngine::DrawMode::Triangle:
			return VkPolygonMode::VK_POLYGON_MODE_FILL;
		case SmolEngine::DrawMode::Line:
			return VkPolygonMode::VK_POLYGON_MODE_LINE;
		case SmolEngine::DrawMode::Fan:
			return VkPolygonMode::VK_POLYGON_MODE_LINE;
		default:
			return VkPolygonMode::VK_POLYGON_MODE_FILL;
		}
	}
}