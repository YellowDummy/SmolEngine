#include "stdafx.h"
#include "VulkanPipeline.h"

#include "Core/Renderer/Vulkan/VulkanPipelineSpecification.h"
#include "Core/Renderer/Vulkan/VulkanContext.h"

namespace SmolEngine
{
	VulkanPipeline::VulkanPipeline()
	{
		m_VulkanPipelineSpecification = new VulkanPipelineSpecification();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		delete m_VulkanPipelineSpecification;
	}

	bool VulkanPipeline::Invalidate(const VulkanPipelineSpecification* pipelineSpec)
	{
		if(!pipelineSpec->Device || !pipelineSpec->Shader || !pipelineSpec->TargetSwapchain || !pipelineSpec->BufferLayout)
		{
			assert(false);
			return false;
		}

		memcpy(m_VulkanPipelineSpecification, pipelineSpec, sizeof(VulkanPipelineSpecification));
		BuildDescriptors(pipelineSpec->Shader, pipelineSpec->Textures);

		const auto& shader = pipelineSpec->Shader;
		const auto& swapchain = pipelineSpec->TargetSwapchain;
		const auto& device = pipelineSpec->Device->GetLogicalDevice();

		VkPipelineLayoutCreateInfo pipelineLayoutCI = {};
		{
			pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCI.pNext = nullptr;
			pipelineLayoutCI.setLayoutCount = 1;
			pipelineLayoutCI.pSetLayouts = &m_DescriptorSetLayout;
			pipelineLayoutCI.pushConstantRangeCount = static_cast<uint32_t>(shader->m_VkPushConstantRanges.size());
			pipelineLayoutCI.pPushConstantRanges = shader->m_VkPushConstantRanges.data();

			VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &m_PipelineLayout));
		}

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
			pipelineCreateInfo.renderPass = swapchain->GetRenderPass();
		}

		// Construct the different states making up the pipeline

		// Input assembly state describes how primitives are assembled
		// This pipeline will assemble vertex data as a triangle lists (though we only use one triangle)
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
		inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		// Rasterization state
		VkPipelineRasterizationStateCreateInfo rasterizationState = {};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationState.cullMode = VK_CULL_MODE_NONE;
		rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.depthBiasEnable = VK_FALSE;
		rasterizationState.lineWidth = 1.0f;

		// Color blend state describes how blend factors are calculated (if used)
		// We need one blend attachment state per color attachment (even if blending is not used)
		VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};
		{
			blendAttachmentState[0].colorWriteMask = 0xf;
			blendAttachmentState[0].blendEnable = VK_FALSE;
			if (pipelineSpec->IsAlphaBlendingEnabled)
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
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = blendAttachmentState;

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
		depthStencilState.depthTestEnable = VK_TRUE;
		depthStencilState.depthWriteEnable = VK_TRUE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
		depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
		depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.front = depthStencilState.back;

		// Multi sampling state
		// This example does not make use of multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.pSampleMask = nullptr;

		// Vertex input descriptions
		// Specifies the vertex input parameters for a pipeline

		// Vertex input binding
        // This example uses a single vertex input binding at binding point 0 (see vkCmdBindVertexBuffers)
		VkVertexInputBindingDescription vertexInputBinding = {};
		vertexInputBinding.binding = 0;
		vertexInputBinding.stride = pipelineSpec->Stride;
		vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		uint32_t index = 0;
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributs(pipelineSpec->BufferLayout->GetElements().size());
		for (const auto& element: pipelineSpec->BufferLayout->GetElements())
		{

			// Attribute location 0: Position
			vertexInputAttributs[index].binding = 0;
			vertexInputAttributs[index].location = index;
			vertexInputAttributs[index].format = VK_FORMAT_R32G32B32_SFLOAT;
			vertexInputAttributs[index].offset = element.offset;

			index++;
		}

		// Vertex input state used for pipeline creation
		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount = 1;
		vertexInputState.pVertexBindingDescriptions = &vertexInputBinding;
		vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributs.size());
		vertexInputState.pVertexAttributeDescriptions = vertexInputAttributs.data();


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
		pipelineCreateInfo.renderPass = swapchain->GetRenderPass();
		pipelineCreateInfo.pDynamicState = &dynamicState;

		VkPipelineCacheCreateInfo pipelineCacheCI = {};
		{
			pipelineCacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		}

		VK_CHECK_RESULT(vkCreatePipelineCache(device, &pipelineCacheCI, nullptr, &m_PipelineCache))

		// Create rendering pipeline using the specified states
		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, m_PipelineCache, 1, &pipelineCreateInfo, nullptr, &m_Pipeline));

		// Shader modules are no longer needed once the graphics pipeline has been created
		//vkDestroyShaderModule(device, shaderStages[0].module, nullptr);
		//vkDestroyShaderModule(device, shaderStages[1].module, nullptr);


		return true;
	}

	bool VulkanPipeline::ReCreate()
	{
		if (m_VulkanPipelineSpecification != nullptr)
		{
			Destroy();
			return Invalidate(m_VulkanPipelineSpecification);
		}

		return false;
	}

	void VulkanPipeline::Destroy()
	{
		const auto& device = VulkanContext::GetDevice().GetLogicalDevice();
		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
		vkDestroyPipelineCache(device, m_PipelineCache, nullptr);
		vkDestroyPipeline(device, m_Pipeline, nullptr);
	}

	void VulkanPipeline::UpdateSamplers2D(const std::vector<VulkanTexture*>& textures, VkCommandBuffer cmdBuffer)
	{
		std::vector< VkDescriptorImageInfo> descriptorImageInfos;
		uint32_t index = 0;
		for (auto& image : textures)
		{
			if (image == nullptr)
			{
				break;
			}

			if (!image->IsActive())
			{
				NATIVE_ERROR("VulkanTexture is not initialized!");
				assert(image->IsActive() == true);
			}

			descriptorImageInfos.push_back(image->m_DescriptorImageInfo);
			index++;
		}

		VkWriteDescriptorSet* samplerSet = nullptr;
		for (auto& writeSet : m_WriteDescriptorSets)
		{
			if (writeSet.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
			{
				samplerSet = &writeSet;
				break;
			}
		}

		if (samplerSet != nullptr)
		{
			uint32_t bindingPoint = samplerSet->dstBinding;
			if (index < m_ReservedTextures.size())
			{
				uint32_t maxSize = m_ReservedTextures.size();
				descriptorImageInfos.reserve(maxSize - index);

				for (uint32_t i = 0; i < maxSize - index; ++i)
				{
					descriptorImageInfos.emplace_back(m_ReservedTextures[i]->m_DescriptorImageInfo);
				}
			}

			*samplerSet = VulkanDescriptor::Create(m_DesciptorSet,
				bindingPoint, descriptorImageInfos, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

			vkUpdateDescriptorSets(VulkanContext::GetDevice().GetLogicalDevice() , static_cast<uint32_t>(m_WriteDescriptorSets.size()), m_WriteDescriptorSets.data(), 0, nullptr);
			// Bind descriptor sets describing shader binding points
			vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1,
				&m_DesciptorSet, 0, nullptr);
		}

	}

	const VkPipeline& VulkanPipeline::GetVkPipeline() const
	{
		return m_Pipeline;
	}

	const VkPipelineLayout& VulkanPipeline::GetVkPipelineLayot() const
	{
		return m_PipelineLayout;
	}

	const VkDescriptorSet VulkanPipeline::GetVkDescriptorSet() const
	{
		return m_DesciptorSet;
	}

	const VkDescriptorSetLayout VulkanPipeline::GetVkDescriptorSetLayout() const
	{
		return m_DescriptorSetLayout;
	}

	void VulkanPipeline::BuildDescriptors(VulkanShader* shader, const std::vector<VulkanTexture*>& textures)
	{
		const auto& device = VulkanContext::GetDevice().GetLogicalDevice();

		m_WriteDescriptorSets.clear();
		if (m_DescriptorSetLayout != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
		}
		if (m_DescriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
		}

		std::vector< VkDescriptorPoolSize> DescriptorPoolSizes;
		std::vector< VkDescriptorSetLayoutBinding> DescriptorSetLayoutBinding;
		uint32_t unformsBufferSize = static_cast<uint32_t>(shader->m_UniformBuffers.size());
		uint32_t resourcesBufferSize = static_cast<uint32_t>(shader->m_UniformResources.size());;

		// Uniform Buffer PoolSize
		if (!shader->m_UniformBuffers.empty())
		{
			VkDescriptorPoolSize tempPoolSize = {};
			{
				tempPoolSize.descriptorCount = unformsBufferSize;
				tempPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			}

			DescriptorPoolSizes.push_back(tempPoolSize);
		}

		// Resources Buffer PoolSize
		if (!shader->m_UniformResources.empty())
		{
			VkDescriptorPoolSize tempPoolSize = {};
			{
				tempPoolSize.descriptorCount = resourcesBufferSize;
				tempPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			}

			DescriptorPoolSizes.push_back(tempPoolSize);
		}

		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		{
			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.pNext = nullptr;
			descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(DescriptorPoolSizes.size());
			descriptorPoolInfo.pPoolSizes = DescriptorPoolSizes.data();
			descriptorPoolInfo.maxSets = static_cast<uint32_t>(DescriptorPoolSizes.size());

			VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &m_DescriptorPool));
		}

		// UBO's

		if (!shader->m_UniformBuffers.empty())
		{
			for (auto& uboInfo : shader->m_UniformBuffers)
			{
				auto& [bindingPoint, buffer] = uboInfo;

				VkDescriptorSetLayoutBinding layoutBinding = {};
				{
					layoutBinding.binding = buffer.BindingPoint;
					layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					layoutBinding.descriptorCount = static_cast<uint32_t>(buffer.Uniforms.size());
					layoutBinding.stageFlags = buffer.StageFlags;
				}

				DescriptorSetLayoutBinding.push_back(layoutBinding);

				buffer.VkBuffer.Create(buffer.Size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
				buffer.DesriptorBufferInfo.buffer = buffer.VkBuffer.GetBuffer();
				buffer.DesriptorBufferInfo.offset = 0;
				buffer.DesriptorBufferInfo.range = buffer.VkBuffer.GetSize();
			}

		}

		// Samplers

		if (!shader->m_UniformResources.empty())
		{

			for (auto& info : shader->m_UniformResources)
			{
				auto& [bindingPoint, res] = info;

				VkDescriptorSetLayoutBinding layoutBinding = {};
				{
					layoutBinding.binding = res.BindingPoint;
					layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					if (res.ArraySize > 0)
					{
						layoutBinding.descriptorCount = res.ArraySize;
					}
					else
					{
						layoutBinding.descriptorCount = 1;
					}
					layoutBinding.stageFlags = res.StageFlags;

					DescriptorSetLayoutBinding.push_back(layoutBinding);
				}
			}
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		{
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = static_cast<uint32_t>(DescriptorSetLayoutBinding.size());
			layoutInfo.pBindings = DescriptorSetLayoutBinding.data();

			VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout));
		}

		VkDescriptorSetAllocateInfo allocateInfo = {};
		{
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.descriptorPool = m_DescriptorPool;
			allocateInfo.descriptorSetCount = 1;
			allocateInfo.pSetLayouts = &m_DescriptorSetLayout;

			VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocateInfo, &m_DesciptorSet));
		}
		
		/// Maybe move somewhere else
		/// Seconds Pass
		{
			/// UBO

			if (!shader->m_UniformBuffers.empty())
			{
				for (auto& uboInfo : shader->m_UniformBuffers)
				{
					auto& [bindingPoint, buffer] = uboInfo;

					m_WriteDescriptorSets.push_back(VulkanDescriptor::Create(m_DesciptorSet,
						buffer.BindingPoint, &buffer.DesriptorBufferInfo));

					NATIVE_WARN("Created UBO {}: Members Count: {}, Binding Point: {}", buffer.Name, buffer.Uniforms.size(), buffer.BindingPoint);
				}
			}

			/// Samplers

			std::vector< VkDescriptorImageInfo> descriptorImageInfos;
			if (!shader->m_UniformResources.empty()) 
			{
				for (auto& [bindingPoint, res] : shader->m_UniformResources)
				{
					if (res.ArraySize > 0)
					{
						descriptorImageInfos.reserve(textures.size());
						uint32_t index = 0;
						for (auto& image : textures)
						{
							if (!image->IsActive())
							{
								NATIVE_ERROR("VulkanTexture is not initialized!");
								assert(image->IsActive() == true);
							}

							descriptorImageInfos.emplace_back(image->m_DescriptorImageInfo);
							index++;
						}

						if (index < res.ArraySize)
						{
							uint32_t count = 0;
							m_ReservedTextures.resize(res.ArraySize - index);
							for (uint32_t i = index; i < res.ArraySize; ++i)
							{
#ifndef SMOLENGINE_OPENGL_IMPL
								m_ReservedTextures[count] = Texture2D::CreateWhiteTexture()->GetVulkanTexture();
#endif
								descriptorImageInfos.emplace_back(m_ReservedTextures[count]->m_DescriptorImageInfo);
								count++;
							}
						}

						m_WriteDescriptorSets.push_back(VulkanDescriptor::Create(m_DesciptorSet,
							res.BindingPoint, descriptorImageInfos, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));

						NATIVE_WARN("UniformResource: BindingPoint: {}, ArraySize: {}", res.BindingPoint, res.ArraySize);
					}
					else
					{
						if (textures.size() > 0)
						{
							auto& texture = *textures[0];
							if (texture.IsActive())
							{
								m_WriteDescriptorSets.push_back(VulkanDescriptor::Create(m_DesciptorSet,
									res.BindingPoint, &texture.m_DescriptorImageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
							}
						}
						else
						{
							m_ReservedTextures.resize(1);
#ifndef SMOLENGINE_OPENGL_IMPL
							m_ReservedTextures[0] = Texture2D::CreateWhiteTexture()->GetVulkanTexture();
#endif

							m_WriteDescriptorSets.push_back(VulkanDescriptor::Create(m_DesciptorSet,
								res.BindingPoint, &m_ReservedTextures[0]->m_DescriptorImageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));

						}

						NATIVE_WARN("UniformResource: BindingPoint: {}, ArraySize: {}", res.BindingPoint, res.ArraySize);
					}
				}
			}

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(m_WriteDescriptorSets.size()), m_WriteDescriptorSets.data(), 0, nullptr);
		}
	}
}