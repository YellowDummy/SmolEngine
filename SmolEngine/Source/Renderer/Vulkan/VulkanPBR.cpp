#include "stdafx.h"
#include "VulkanPBR.h"

#include "Renderer/Vulkan/VulkanTexture.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanMemoryAllocator.h"
#include "Renderer/Vulkan/VulkanShader.h"
#include "Renderer/Vulkan/VulkanCommandBuffer.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/Buffer.h"
#include "Renderer/CubeTexture.h"

namespace SmolEngine
{
#define M_PI       3.14159265358979323846   // pi

	void VulkanPBR::GenerateBRDFLUT(VkImage outImage, VkImageView outImageView, VkSampler outSampler,
		VkDeviceMemory outImageMem, VkDescriptorImageInfo& outImageInfo)
	{
		auto start = std::chrono::high_resolution_clock::now();
		VkDevice device = VulkanContext::GetDevice().GetLogicalDevice();

		// R16G16 is supported pretty much everywhere
		const VkFormat format = VK_FORMAT_R16G16_SFLOAT;
		const int32_t dim = 512;

		// Image
		VkImageCreateInfo imageCI = {};
		{
			imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCI.imageType = VK_IMAGE_TYPE_2D;
			imageCI.format = format;
			imageCI.extent.width = dim;
			imageCI.extent.height = dim;
			imageCI.extent.depth = 1;
			imageCI.mipLevels = 1;
			imageCI.arrayLayers = 1;
			imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCI.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			VK_CHECK_RESULT(vkCreateImage(device, &imageCI, nullptr, &outImage));

			VkMemoryRequirements memReqs;
			vkGetImageMemoryRequirements(device, outImage, &memReqs);
			uint32_t typeIndex = VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			VulkanMemoryAllocator::Allocate(device, memReqs, &outImageMem, typeIndex);
			VK_CHECK_RESULT(vkBindImageMemory(device, outImage, outImageMem, 0));
		}

		// View
		{
			VkImageViewCreateInfo viewCI = {};
			viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewCI.format = format;
			viewCI.subresourceRange = {};
			viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewCI.subresourceRange.levelCount = 1;
			viewCI.subresourceRange.layerCount = 1;
			viewCI.image = outImage;
			VK_CHECK_RESULT(vkCreateImageView(device, &viewCI, nullptr, &outImageView));
		}

		//Sampler
		{
			VkSamplerCreateInfo samplerCI = {};
			samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerCI.maxAnisotropy = 1.0f;
			samplerCI.magFilter = VK_FILTER_LINEAR;
			samplerCI.minFilter = VK_FILTER_LINEAR;
			samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerCI.minLod = 0.0f;
			samplerCI.maxLod = 1.0f;
			samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			VK_CHECK_RESULT(vkCreateSampler(device, &samplerCI, nullptr, &outSampler));
		}

		// FB, Att, RP, Pipe
		{
			VkAttachmentDescription attDesc = {};

			// Color attachment
			attDesc.format = format;
			attDesc.samples = VK_SAMPLE_COUNT_1_BIT;
			attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

			VkSubpassDescription subpassDescription = {};
			{
				subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpassDescription.colorAttachmentCount = 1;
				subpassDescription.pColorAttachments = &colorReference;
			}

			// Use subpass dependencies for layout transitions
			std::array<VkSubpassDependency, 2> dependencies;
			{
				dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
				dependencies[0].dstSubpass = 0;
				dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

				dependencies[1].srcSubpass = 0;
				dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
				dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			}

			// Create the actual renderpass
			VkRenderPassCreateInfo renderPassCI = {};
			{
				renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassCI.attachmentCount = 1;
				renderPassCI.pAttachments = &attDesc;
				renderPassCI.subpassCount = 1;
				renderPassCI.pSubpasses = &subpassDescription;
				renderPassCI.dependencyCount = 2;
				renderPassCI.pDependencies = dependencies.data();
			}

			VkRenderPass renderpass;
			VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassCI, nullptr, &renderpass));

			// Framebuffer
			VkFramebufferCreateInfo framebufferCI = {};
			{
				framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferCI.renderPass = renderpass;
				framebufferCI.attachmentCount = 1;
				framebufferCI.pAttachments = &outImageView;
				framebufferCI.width = dim;
				framebufferCI.height = dim;
				framebufferCI.layers = 1;
			}

			VkFramebuffer framebuffer;
			VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferCI, nullptr, &framebuffer));

			// Shader
			VulkanShader shader = {};
			{
				assert(shader.Init("../Resources/Shaders/Gen_BRD_flut_Vertex.glsl", "../Resources/Shaders/Gen_BRD_flut_Frag.glsl") == true);
			}

			// Descriptors
			VkDescriptorSetLayout descriptorsetlayout;
			VkDescriptorPool descriptorpool;
			VkDescriptorSet descriptorset;
			{
				VkDescriptorSetLayoutCreateInfo descriptorsetlayoutCI = {};
				{
					descriptorsetlayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
					descriptorsetlayoutCI.pBindings = nullptr;
					descriptorsetlayoutCI.bindingCount = 0;
				}

				VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorsetlayoutCI, nullptr, &descriptorsetlayout));

				VkDescriptorPoolSize tempPoolSize = {};
				{
					tempPoolSize.descriptorCount = 1;
					tempPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				}

				VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
				{
					descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
					descriptorPoolInfo.pNext = nullptr;
					descriptorPoolInfo.poolSizeCount = 1;
					descriptorPoolInfo.pPoolSizes = &tempPoolSize;
					descriptorPoolInfo.maxSets = 2;

					VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorpool));
				}

				VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
				{
					descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
					descriptorSetAllocateInfo.descriptorPool = descriptorpool;
					descriptorSetAllocateInfo.pSetLayouts = &descriptorsetlayout;
					descriptorSetAllocateInfo.descriptorSetCount = 1;
				}
				VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorset));

			}

			// Pipeline Layout
			VkPipelineLayout pipelinelayout;
			{
				VkPipelineLayoutCreateInfo pipelineLayoutCI = {};
				{
					pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
					pipelineLayoutCI.pNext = nullptr;
					pipelineLayoutCI.setLayoutCount = 1;
					pipelineLayoutCI.pSetLayouts = &descriptorsetlayout;
					pipelineLayoutCI.pushConstantRangeCount = static_cast<uint32_t>(shader.m_VkPushConstantRanges.size());
					pipelineLayoutCI.pPushConstantRanges = shader.m_VkPushConstantRanges.data();

					VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelinelayout));
				}
			}

			// Pipeline
			VkPipelineCache cache;
			VkPipeline pipeline;
			VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
			{
				VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
				inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

				VkPipelineRasterizationStateCreateInfo rasterizationState = {};
				rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
				rasterizationState.cullMode = VK_CULL_MODE_NONE;
				rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
				rasterizationState.depthClampEnable = VK_FALSE;
				rasterizationState.lineWidth = 1.0f;

				VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};
				{
					blendAttachmentState[0].colorWriteMask = 0xf;
					blendAttachmentState[0].blendEnable = VK_FALSE;
				}

				VkPipelineColorBlendStateCreateInfo colorBlendState = {};
				colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlendState.attachmentCount = 1;
				colorBlendState.pAttachments = blendAttachmentState;

				VkPipelineViewportStateCreateInfo viewportState = {};
				viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				viewportState.viewportCount = 1;
				viewportState.scissorCount = 1;

				std::vector<VkDynamicState> dynamicStateEnables;
				dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
				dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);

				VkPipelineDynamicStateCreateInfo dynamicState = {};
				dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				dynamicState.pDynamicStates = dynamicStateEnables.data();
				dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

				VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
				depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				depthStencilState.depthTestEnable = VK_FALSE;
				depthStencilState.depthWriteEnable = VK_FALSE;
				depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
				depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

				VkPipelineMultisampleStateCreateInfo multisampleState = {};
				multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

				VkPipelineVertexInputStateCreateInfo vertexInputState = {};
				vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

				pipelineCreateInfo.stageCount = static_cast<uint32_t>(shader.GetVkPipelineShaderStages().size());
				pipelineCreateInfo.pStages = shader.GetVkPipelineShaderStages().data();

				// Assign the pipeline states to the pipeline creation info structure

				pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipelineCreateInfo.layout = pipelinelayout;
				pipelineCreateInfo.pVertexInputState = &vertexInputState;
				pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
				pipelineCreateInfo.pRasterizationState = &rasterizationState;
				pipelineCreateInfo.pColorBlendState = &colorBlendState;
				pipelineCreateInfo.pMultisampleState = &multisampleState;
				pipelineCreateInfo.pViewportState = &viewportState;
				pipelineCreateInfo.pDepthStencilState = &depthStencilState;
				pipelineCreateInfo.renderPass = renderpass;
				pipelineCreateInfo.pDynamicState = &dynamicState;

				VkPipelineCacheCreateInfo pipelineCacheCI = {};
				{
					pipelineCacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
				}

				VK_CHECK_RESULT(vkCreatePipelineCache(device, &pipelineCacheCI, nullptr, &cache));
				VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, cache, 1, &pipelineCreateInfo, nullptr, &pipeline));
			}

			//Render
			{
				VkClearValue clearValues[1];
				clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

				VkRenderPassBeginInfo renderPassBeginInfo = {};
				{
					renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
					renderPassBeginInfo.renderPass = renderpass;
					renderPassBeginInfo.renderArea.extent.width = dim;
					renderPassBeginInfo.renderArea.extent.height = dim;
					renderPassBeginInfo.clearValueCount = 1;
					renderPassBeginInfo.pClearValues = clearValues;
					renderPassBeginInfo.framebuffer = framebuffer;
				}

				VkCommandBuffer cmdBuffer = VulkanCommandBuffer::CreateSingleCommandBuffer();
				{
					vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
					VkViewport viewport = {};
					{
						viewport.width = (float)dim;
						viewport.height = (float)dim;
						viewport.minDepth = 0.0f;
						viewport.maxDepth = 1.0f;
					}

					VkRect2D rect2D = {};
					{
						rect2D.extent.width = dim;
						rect2D.extent.height = dim;
						rect2D.offset.x = 0;
						rect2D.offset.y = 0;
					}

					vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
					vkCmdSetScissor(cmdBuffer, 0, 1, &rect2D);
					vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
					vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
					vkCmdEndRenderPass(cmdBuffer);
				}
				VulkanCommandBuffer::EndSingleCommandBuffer(cmdBuffer);

			}

			outImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			outImageInfo.imageView = outImageView;
			outImageInfo.sampler = outSampler;

			vkDestroyPipeline(device, pipeline, nullptr);
			vkDestroyPipelineLayout(device, pipelinelayout, nullptr);
			vkDestroyRenderPass(device, renderpass, nullptr);
			vkDestroyFramebuffer(device, framebuffer, nullptr);
			vkDestroyDescriptorSetLayout(device, descriptorsetlayout, nullptr);
			vkDestroyDescriptorPool(device, descriptorpool, nullptr);
		}

		auto end = std::chrono::high_resolution_clock::now();
		double diff = std::chrono::duration<double, std::milli>(end - start).count();

		NATIVE_INFO("Generating BRDF LUT took {} ms", diff);
	}

	void VulkanPBR::GenerateIrradianceCube(VkImage outImage, VkImageView outImageView, VkSampler outSampler,
		VkDeviceMemory outImageMem, VulkanTexture* skyBox, VkDescriptorImageInfo& outImageInfo)
	{
		auto start = std::chrono::high_resolution_clock::now();
		VkDevice device = VulkanContext::GetDevice().GetLogicalDevice();

		const VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
		const int32_t dim = 64;
		const uint32_t numMips = static_cast<uint32_t>(floor(log2(dim))) + 1;

		// Image
		VkImageCreateInfo imageCI = {};
		{
			imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCI.imageType = VK_IMAGE_TYPE_2D;
			imageCI.format = format;
			imageCI.extent.width = dim;
			imageCI.extent.height = dim;
			imageCI.extent.depth = 1;
			imageCI.mipLevels = numMips;
			imageCI.arrayLayers = 6;
			imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCI.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			imageCI.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
			VK_CHECK_RESULT(vkCreateImage(device, &imageCI, nullptr, &outImage));

			VkMemoryRequirements memReqs;
			vkGetImageMemoryRequirements(device, outImage, &memReqs);
			uint32_t typeIndex = VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			VulkanMemoryAllocator::Allocate(device, memReqs, &outImageMem, typeIndex);
			VK_CHECK_RESULT(vkBindImageMemory(device, outImage, outImageMem, 0));
		}

		// View
		{
			VkImageViewCreateInfo viewCI = {};
			viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewCI.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
			viewCI.format = format;
			viewCI.subresourceRange = {};
			viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewCI.subresourceRange.levelCount = numMips;
			viewCI.subresourceRange.layerCount = 6;
			viewCI.image = outImage;
			VK_CHECK_RESULT(vkCreateImageView(device, &viewCI, nullptr, &outImageView));
		}

		//Sampler
		{
			VkSamplerCreateInfo samplerCI = {};
			samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerCI.maxAnisotropy = 1.0f;
			samplerCI.magFilter = VK_FILTER_LINEAR;
			samplerCI.minFilter = VK_FILTER_LINEAR;
			samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerCI.minLod = 0.0f;
			samplerCI.maxLod = static_cast<float>(numMips);
			samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			VK_CHECK_RESULT(vkCreateSampler(device, &samplerCI, nullptr, &outSampler));
		}

		// FB, Att, RP, Pipe
		{
			VkAttachmentDescription attDesc = {};

			// Color attachment
			attDesc.format = format;
			attDesc.samples = VK_SAMPLE_COUNT_1_BIT;
			attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

			VkSubpassDescription subpassDescription = {};
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.colorAttachmentCount = 1;
			subpassDescription.pColorAttachments = &colorReference;

			// Use subpass dependencies for layout transitions
			std::array<VkSubpassDependency, 2> dependencies;
			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			// Renderpass
			VkRenderPass renderpass;
			VkRenderPassCreateInfo renderPassCI = {};
			{
				renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassCI.attachmentCount = 1;
				renderPassCI.pAttachments = &attDesc;
				renderPassCI.subpassCount = 1;
				renderPassCI.pSubpasses = &subpassDescription;
				renderPassCI.dependencyCount = 2;
				renderPassCI.pDependencies = dependencies.data();
			}

			VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassCI, nullptr, &renderpass));

			struct {
				VkImage image;
				VkImageView view;
				VkDeviceMemory memory;
				VkFramebuffer framebuffer;
			} offscreen;

			// Offfscreen framebuffer
			{
				// Color attachment
				VkImageCreateInfo imageCreateInfo = {};
				{
					imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
					imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
					imageCreateInfo.format = format;
					imageCreateInfo.extent.width = dim;
					imageCreateInfo.extent.height = dim;
					imageCreateInfo.extent.depth = 1;
					imageCreateInfo.mipLevels = 1;
					imageCreateInfo.arrayLayers = 1;
					imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
					imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
					imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
					imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

					VK_CHECK_RESULT(vkCreateImage(device, &imageCreateInfo, nullptr, &offscreen.image));

					VkMemoryRequirements memReqs;
					vkGetImageMemoryRequirements(device, offscreen.image, &memReqs);
					uint32_t typeIndex = VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
					VulkanMemoryAllocator::Allocate(device, memReqs, &offscreen.memory, typeIndex);
					VK_CHECK_RESULT(vkBindImageMemory(device, offscreen.image, offscreen.memory, 0));
				}

				VkImageViewCreateInfo colorImageView = {};
				{
					colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
					colorImageView.format = format;
					colorImageView.flags = 0;
					colorImageView.subresourceRange = {};
					colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					colorImageView.subresourceRange.baseMipLevel = 0;
					colorImageView.subresourceRange.levelCount = 1;
					colorImageView.subresourceRange.baseArrayLayer = 0;
					colorImageView.subresourceRange.layerCount = 1;
					colorImageView.image = offscreen.image;
					VK_CHECK_RESULT(vkCreateImageView(device, &colorImageView, nullptr, &offscreen.view));
				}

				VkFramebufferCreateInfo fbufCreateInfo = {};
				{
					fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
					fbufCreateInfo.renderPass = renderpass;
					fbufCreateInfo.attachmentCount = 1;
					fbufCreateInfo.pAttachments = &offscreen.view;
					fbufCreateInfo.width = dim;
					fbufCreateInfo.height = dim;
					fbufCreateInfo.layers = 1;
					VK_CHECK_RESULT(vkCreateFramebuffer(device, &fbufCreateInfo, nullptr, &offscreen.framebuffer));
				}

				VkCommandBuffer layoutCmd = VulkanCommandBuffer::CreateSingleCommandBuffer();
				{
					VulkanTexture::SetImageLayout(
						layoutCmd,
						offscreen.image,
						VK_IMAGE_ASPECT_COLOR_BIT,
						VK_IMAGE_LAYOUT_UNDEFINED,
						VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
				}
				VulkanCommandBuffer::FlushCommandBuffer(layoutCmd);
			}

			// Descriptors
			VkDescriptorSetLayout descriptorsetlayout;
			VkDescriptorPool descriptorpool;
			VkDescriptorSet descriptorset;
			{
				VkDescriptorSetLayoutBinding setLayoutBinding = {};
				{
					setLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					setLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
					setLayoutBinding.binding = 0;
					setLayoutBinding.descriptorCount = 1;
				}

				VkDescriptorSetLayoutCreateInfo descriptorsetlayoutCI = {};
				{
					descriptorsetlayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
					descriptorsetlayoutCI.pBindings = &setLayoutBinding;
					descriptorsetlayoutCI.bindingCount = 1;
				}

				VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorsetlayoutCI, nullptr, &descriptorsetlayout));

				VkDescriptorPoolSize tempPoolSize = {};
				{
					tempPoolSize.descriptorCount = 1;
					tempPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				}

				VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
				{
					descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
					descriptorPoolInfo.pNext = nullptr;
					descriptorPoolInfo.poolSizeCount = 1;
					descriptorPoolInfo.pPoolSizes = &tempPoolSize;
					descriptorPoolInfo.maxSets = 2;

					VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorpool));
				}

				VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
				{
					descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
					descriptorSetAllocateInfo.descriptorPool = descriptorpool;
					descriptorSetAllocateInfo.pSetLayouts = &descriptorsetlayout;
					descriptorSetAllocateInfo.descriptorSetCount = 1;
				}
				VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorset));


				VkWriteDescriptorSet writeDescriptorSet = {};
				{
					writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					writeDescriptorSet.dstSet = descriptorset;
					writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					writeDescriptorSet.dstBinding = 0;
					writeDescriptorSet.pImageInfo = &skyBox->m_DescriptorImageInfo;
					writeDescriptorSet.descriptorCount = 1;
				}
				vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);

			}

			// PushConstant
			struct PushBlock {
				glm::mat4 mvp;
				// Sampling deltas
				float deltaPhi = (2.0f * float(M_PI)) / 180.0f;
				float deltaTheta = (0.5f * float(M_PI)) / 64.0f;
			} pushBlock;


			// Shader
			VulkanShader shader = {};
			{
				assert(shader.Init("../Resources/Shaders/FilterCube_Vulkan_Vertex.glsl",
					"../Resources/Shaders/IrradianceCube_Vulkan_Frag.glsl") == true);
			}

			// Pipeline Layout
			VkPipelineLayout pipelinelayout;
			{
				VkPushConstantRange pushConstantRange{};
				{
					pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
					pushConstantRange.offset = 0;
					pushConstantRange.size = sizeof(PushBlock);
				}

				VkPipelineLayoutCreateInfo pipelineLayoutCI = {};
				{
					pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
					pipelineLayoutCI.pNext = nullptr;
					pipelineLayoutCI.setLayoutCount = 1;
					pipelineLayoutCI.pSetLayouts = &descriptorsetlayout;
					pipelineLayoutCI.pushConstantRangeCount = 1;
					pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;

					VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelinelayout));
				}
			}

			// Pipeline
			VkPipelineCache cache;
			VkPipeline pipeline;
			VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
			{
				VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
				inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

				VkPipelineRasterizationStateCreateInfo rasterizationState = {};
				rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
				rasterizationState.cullMode = VK_CULL_MODE_NONE;
				rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
				rasterizationState.depthClampEnable = VK_FALSE;
				rasterizationState.lineWidth = 1.0f;

				VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};
				{
					blendAttachmentState[0].colorWriteMask = 0xf;
					blendAttachmentState[0].blendEnable = VK_FALSE;
				}

				VkPipelineColorBlendStateCreateInfo colorBlendState = {};
				colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlendState.attachmentCount = 1;
				colorBlendState.pAttachments = blendAttachmentState;

				VkPipelineViewportStateCreateInfo viewportState = {};
				viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				viewportState.viewportCount = 1;
				viewportState.scissorCount = 1;

				std::vector<VkDynamicState> dynamicStateEnables;
				dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
				dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);

				VkPipelineDynamicStateCreateInfo dynamicState = {};
				dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				dynamicState.pDynamicStates = dynamicStateEnables.data();
				dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

				VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
				depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				depthStencilState.depthTestEnable = VK_FALSE;
				depthStencilState.depthWriteEnable = VK_FALSE;
				depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
				depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

				VkPipelineMultisampleStateCreateInfo multisampleState = {};
				multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

				BufferLayout layout(
					{
						{ ShaderDataType::Float3, "a_Position" }
					});

				struct VertexData
				{
					glm::vec3 pos;
				};

				VkVertexInputBindingDescription vertexInputBinding = {};
				vertexInputBinding.binding = 0;
				vertexInputBinding.stride = sizeof(VertexData);
				vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				std::vector<VkVertexInputAttributeDescription> vertexInputAttributs(layout.GetElements().size());
				{
					uint32_t index = 0;
					for (const auto& element : layout.GetElements())
					{
						vertexInputAttributs[index].binding = 0;
						vertexInputAttributs[index].location = index;
						vertexInputAttributs[index].format = VK_FORMAT_R32G32B32_SFLOAT; // TODO: add more formats!
						vertexInputAttributs[index].offset = element.offset;
						index++;
					}
				}

				// Vertex input state used for pipeline creation
				VkPipelineVertexInputStateCreateInfo vertexInputState = {};
				vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vertexInputState.vertexBindingDescriptionCount = 1;
				vertexInputState.pVertexBindingDescriptions = &vertexInputBinding;
				vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributs.size());
				vertexInputState.pVertexAttributeDescriptions = vertexInputAttributs.data();

				pipelineCreateInfo.stageCount = static_cast<uint32_t>(shader.GetVkPipelineShaderStages().size());
				pipelineCreateInfo.pStages = shader.GetVkPipelineShaderStages().data();

				// Assign the pipeline states to the pipeline creation info structure

				pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipelineCreateInfo.layout = pipelinelayout;
				pipelineCreateInfo.pVertexInputState = &vertexInputState;
				pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
				pipelineCreateInfo.pRasterizationState = &rasterizationState;
				pipelineCreateInfo.pColorBlendState = &colorBlendState;
				pipelineCreateInfo.pMultisampleState = &multisampleState;
				pipelineCreateInfo.pViewportState = &viewportState;
				pipelineCreateInfo.pDepthStencilState = &depthStencilState;
				pipelineCreateInfo.renderPass = renderpass;
				pipelineCreateInfo.pDynamicState = &dynamicState;

				VkPipelineCacheCreateInfo pipelineCacheCI = {};
				{
					pipelineCacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
				}

				VK_CHECK_RESULT(vkCreatePipelineCache(device, &pipelineCacheCI, nullptr, &cache));
				VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, cache, 1, &pipelineCreateInfo, nullptr, &pipeline));
			}

			// Render

			VkClearValue clearValues[1];
			clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 0.0f } };


			VkRenderPassBeginInfo renderPassBeginInfo = {};
			{
				renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassBeginInfo.renderPass = renderpass;
				renderPassBeginInfo.framebuffer = offscreen.framebuffer;
				renderPassBeginInfo.renderArea.extent.width = dim;
				renderPassBeginInfo.renderArea.extent.height = dim;
				renderPassBeginInfo.clearValueCount = 1;
				renderPassBeginInfo.pClearValues = clearValues;
			}

			std::vector<glm::mat4> matrices = {
				// POSITIVE_X
				glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
				// NEGATIVE_X
				glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
				// POSITIVE_Y
				glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
				// NEGATIVE_Y
				glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
				// POSITIVE_Z
				glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
				// NEGATIVE_Z
				glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			};

			float skyboxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};

			Ref<VertexBuffer> skyBoxFB = VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices));

			VkCommandBuffer cmdBuffer = VulkanCommandBuffer::CreateSingleCommandBuffer();
			{
				VkViewport viewport = {};
				{
					viewport.width = (float)dim;
					viewport.height = (float)dim;
					viewport.minDepth = 0.0f;
					viewport.maxDepth = 1.0f;
				}

				VkRect2D rect2D = {};
				{
					rect2D.extent.width = dim;
					rect2D.extent.height = dim;
					rect2D.offset.x = 0;
					rect2D.offset.y = 0;
				}

				vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
				vkCmdSetScissor(cmdBuffer, 0, 1, &rect2D);

				VkImageSubresourceRange subresourceRange = {};
				subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				subresourceRange.baseMipLevel = 0;
				subresourceRange.levelCount = numMips;
				subresourceRange.layerCount = 6;


				VulkanTexture::SetImageLayout(cmdBuffer,
					outImage,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					subresourceRange);

				for (uint32_t m = 0; m < numMips; m++)
				{
					for (uint32_t f = 0; f < 6; f++)
					{
						viewport.width = static_cast<float>(dim * std::pow(0.5f, m));
						viewport.height = static_cast<float>(dim * std::pow(0.5f, m));
						vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

						// Render scene from cube face's point of view
						vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

						// Update shader push constant block
						pushBlock.mvp = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];


						vkCmdPushConstants(cmdBuffer, pipelinelayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
							0, sizeof(PushBlock), &pushBlock);

						vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
						vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelinelayout, 0, 1, &descriptorset, 0, NULL);

						VkDeviceSize offsets[1] = { 0 };
						vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &skyBoxFB->GetVulkanVertexBuffer().GetBuffer(), offsets);

						vkCmdDraw(cmdBuffer, 36, 1, 0, 0);

						vkCmdEndRenderPass(cmdBuffer);

						VulkanTexture::SetImageLayout(cmdBuffer,
							offscreen.image,
							VK_IMAGE_ASPECT_COLOR_BIT,
							VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
							VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

						// Copy region for transfer from framebuffer to cube face
						VkImageCopy copyRegion = {};

						copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						copyRegion.srcSubresource.baseArrayLayer = 0;
						copyRegion.srcSubresource.mipLevel = 0;
						copyRegion.srcSubresource.layerCount = 1;
						copyRegion.srcOffset = { 0, 0, 0 };

						copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						copyRegion.dstSubresource.baseArrayLayer = f;
						copyRegion.dstSubresource.mipLevel = m;
						copyRegion.dstSubresource.layerCount = 1;
						copyRegion.dstOffset = { 0, 0, 0 };

						copyRegion.extent.width = static_cast<uint32_t>(viewport.width);
						copyRegion.extent.height = static_cast<uint32_t>(viewport.height);
						copyRegion.extent.depth = 1;

						vkCmdCopyImage(
							cmdBuffer,
							offscreen.image,
							VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
							outImage,
							VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							1,
							&copyRegion);


						VulkanTexture::SetImageLayout(cmdBuffer,
							offscreen.image,
							VK_IMAGE_ASPECT_COLOR_BIT,
							VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
							VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
					}
				}


				VulkanTexture::SetImageLayout(cmdBuffer,
					outImage,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					subresourceRange);
			}
			VulkanCommandBuffer::FlushCommandBuffer(cmdBuffer);

			outImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			outImageInfo.imageView = outImageView;
			outImageInfo.sampler = outSampler;

			vkDestroyRenderPass(device, renderpass, nullptr);
			vkDestroyFramebuffer(device, offscreen.framebuffer, nullptr);
			vkFreeMemory(device, offscreen.memory, nullptr);
			vkDestroyImageView(device, offscreen.view, nullptr);
			vkDestroyImage(device, offscreen.image, nullptr);
			vkDestroyDescriptorPool(device, descriptorpool, nullptr);
			vkDestroyDescriptorSetLayout(device, descriptorsetlayout, nullptr);
			vkDestroyPipeline(device, pipeline, nullptr);
			vkDestroyPipelineLayout(device, pipelinelayout, nullptr);
		}

		auto end = std::chrono::high_resolution_clock::now();
		double diff = std::chrono::duration<double, std::milli>(end - start).count();

		NATIVE_INFO("Generating irradiance cube with {} mip levels took {} ms", numMips, diff);
	}

	void VulkanPBR::GeneratePrefilteredCube(VkImage outImage, VkImageView outImageView, VkSampler outSampler,
		VkDeviceMemory outImageMem, VulkanTexture* skyBox, VkDescriptorImageInfo& outImageInfo)
	{
		auto start = std::chrono::high_resolution_clock::now();
		VkDevice device = VulkanContext::GetDevice().GetLogicalDevice();

		const VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
		const int32_t dim = 512;
		const uint32_t numMips = static_cast<uint32_t>(floor(log2(dim))) + 1;

		// Image
		VkImageCreateInfo imageCI = {};
		{
			imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCI.imageType = VK_IMAGE_TYPE_2D;
			imageCI.format = format;
			imageCI.extent.width = dim;
			imageCI.extent.height = dim;
			imageCI.extent.depth = 1;
			imageCI.mipLevels = numMips;
			imageCI.arrayLayers = 6;
			imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCI.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			imageCI.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
			VK_CHECK_RESULT(vkCreateImage(device, &imageCI, nullptr, &outImage));

			VkMemoryRequirements memReqs;
			vkGetImageMemoryRequirements(device, outImage, &memReqs);
			uint32_t typeIndex = VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			VulkanMemoryAllocator::Allocate(device, memReqs, &outImageMem, typeIndex);
			VK_CHECK_RESULT(vkBindImageMemory(device, outImage, outImageMem, 0));
		}

		// View
		{
			VkImageViewCreateInfo viewCI = {};
			viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewCI.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
			viewCI.format = format;
			viewCI.subresourceRange = {};
			viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewCI.subresourceRange.levelCount = numMips;
			viewCI.subresourceRange.layerCount = 6;
			viewCI.image = outImage;
			VK_CHECK_RESULT(vkCreateImageView(device, &viewCI, nullptr, &outImageView));
		}

		//Sampler
		{
			VkSamplerCreateInfo samplerCI = {};
			samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerCI.maxAnisotropy = 1.0f;
			samplerCI.magFilter = VK_FILTER_LINEAR;
			samplerCI.minFilter = VK_FILTER_LINEAR;
			samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerCI.minLod = 0.0f;
			samplerCI.maxLod = static_cast<float>(numMips);
			samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			VK_CHECK_RESULT(vkCreateSampler(device, &samplerCI, nullptr, &outSampler));
		}

		// FB, Att, RP, Pipe
		{
			VkAttachmentDescription attDesc = {};

			// Color attachment
			attDesc.format = format;
			attDesc.samples = VK_SAMPLE_COUNT_1_BIT;
			attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

			VkSubpassDescription subpassDescription = {};
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.colorAttachmentCount = 1;
			subpassDescription.pColorAttachments = &colorReference;

			// Use subpass dependencies for layout transitions
			std::array<VkSubpassDependency, 2> dependencies;
			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			// Renderpass
			VkRenderPassCreateInfo renderPassCI = {};
			VkRenderPass renderpass;
			{
				renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassCI.attachmentCount = 1;
				renderPassCI.pAttachments = &attDesc;
				renderPassCI.subpassCount = 1;
				renderPassCI.pSubpasses = &subpassDescription;
				renderPassCI.dependencyCount = 2;
				renderPassCI.pDependencies = dependencies.data();
				VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassCI, nullptr, &renderpass));
			}

			struct {
				VkImage image;
				VkImageView view;
				VkDeviceMemory memory;
				VkFramebuffer framebuffer;
			} offscreen;

			// Offfscreen framebuffer
			{
				// Color attachment
				VkImageCreateInfo imageCreateInfo = {};
				{
					imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
					imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
					imageCreateInfo.format = format;
					imageCreateInfo.extent.width = dim;
					imageCreateInfo.extent.height = dim;
					imageCreateInfo.extent.depth = 1;
					imageCreateInfo.mipLevels = 1;
					imageCreateInfo.arrayLayers = 1;
					imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
					imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
					imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
					imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

					VK_CHECK_RESULT(vkCreateImage(device, &imageCreateInfo, nullptr, &offscreen.image));

					VkMemoryRequirements memReqs;
					vkGetImageMemoryRequirements(device, offscreen.image, &memReqs);
					uint32_t typeIndex = VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
					VulkanMemoryAllocator::Allocate(device, memReqs, &offscreen.memory, typeIndex);
					VK_CHECK_RESULT(vkBindImageMemory(device, offscreen.image, offscreen.memory, 0));
				}

				VkImageViewCreateInfo colorImageView = {};
				{
					colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
					colorImageView.format = format;
					colorImageView.flags = 0;
					colorImageView.subresourceRange = {};
					colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					colorImageView.subresourceRange.baseMipLevel = 0;
					colorImageView.subresourceRange.levelCount = 1;
					colorImageView.subresourceRange.baseArrayLayer = 0;
					colorImageView.subresourceRange.layerCount = 1;
					colorImageView.image = offscreen.image;
					VK_CHECK_RESULT(vkCreateImageView(device, &colorImageView, nullptr, &offscreen.view));
				}

				VkFramebufferCreateInfo fbufCreateInfo = {};
				{
					fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
					fbufCreateInfo.renderPass = renderpass;
					fbufCreateInfo.attachmentCount = 1;
					fbufCreateInfo.pAttachments = &offscreen.view;
					fbufCreateInfo.width = dim;
					fbufCreateInfo.height = dim;
					fbufCreateInfo.layers = 1;
					VK_CHECK_RESULT(vkCreateFramebuffer(device, &fbufCreateInfo, nullptr, &offscreen.framebuffer));
				}

				VkCommandBuffer layoutCmd = VulkanCommandBuffer::CreateSingleCommandBuffer();
				{
					VulkanTexture::SetImageLayout(
						layoutCmd,
						offscreen.image,
						VK_IMAGE_ASPECT_COLOR_BIT,
						VK_IMAGE_LAYOUT_UNDEFINED,
						VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
				}
				VulkanCommandBuffer::FlushCommandBuffer(layoutCmd);
			}

			// Descriptors
			VkDescriptorSetLayout descriptorsetlayout;
			VkDescriptorPool descriptorpool;
			VkDescriptorSet descriptorset;
			{
				VkDescriptorSetLayoutBinding setLayoutBinding = {};
				{
					setLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					setLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
					setLayoutBinding.binding = 0;
					setLayoutBinding.descriptorCount = 1;
				}

				VkDescriptorSetLayoutCreateInfo descriptorsetlayoutCI = {};
				{
					descriptorsetlayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
					descriptorsetlayoutCI.pBindings = &setLayoutBinding;
					descriptorsetlayoutCI.bindingCount = 1;
				}

				VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorsetlayoutCI, nullptr, &descriptorsetlayout));

				VkDescriptorPoolSize tempPoolSize = {};
				{
					tempPoolSize.descriptorCount = 1;
					tempPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				}

				VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
				{
					descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
					descriptorPoolInfo.pNext = nullptr;
					descriptorPoolInfo.poolSizeCount = 1;
					descriptorPoolInfo.pPoolSizes = &tempPoolSize;
					descriptorPoolInfo.maxSets = 2;

					VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorpool));
				}

				VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
				{
					descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
					descriptorSetAllocateInfo.descriptorPool = descriptorpool;
					descriptorSetAllocateInfo.pSetLayouts = &descriptorsetlayout;
					descriptorSetAllocateInfo.descriptorSetCount = 1;
				}
				VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorset));


				VkWriteDescriptorSet writeDescriptorSet = {};
				{
					writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					writeDescriptorSet.dstSet = descriptorset;
					writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					writeDescriptorSet.dstBinding = 0;
					writeDescriptorSet.pImageInfo = &skyBox->m_DescriptorImageInfo;
					writeDescriptorSet.descriptorCount = 1;
				}
				vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);

			}

			//PushConstant
			struct PushBlock {
				glm::mat4 mvp;
				float roughness;
				uint32_t numSamples = 32u;
			} pushBlock;

			// Shader
			VulkanShader shader = {};
			{
				assert(shader.Init("../Resources/Shaders/FilterCube_Vulkan_Vertex.glsl",
					"../Resources/Shaders/PreFilterenvMap_Vulkan_Frag.glsl") == true);
			}

			// Pipeline Layout
			VkPipelineLayout pipelinelayout;
			{
				VkPushConstantRange pushConstantRange{};
				{
					pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
					pushConstantRange.offset = 0;
					pushConstantRange.size = sizeof(PushBlock);
				}

				VkPipelineLayoutCreateInfo pipelineLayoutCI = {};
				{
					pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
					pipelineLayoutCI.pNext = nullptr;
					pipelineLayoutCI.setLayoutCount = 1;
					pipelineLayoutCI.pSetLayouts = &descriptorsetlayout;
					pipelineLayoutCI.pushConstantRangeCount = 1;
					pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;

					VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelinelayout));
				}
			}

			// Pipeline
			VkPipelineCache cache;
			VkPipeline pipeline;
			VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
			{
				VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
				inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

				VkPipelineRasterizationStateCreateInfo rasterizationState = {};
				rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
				rasterizationState.cullMode = VK_CULL_MODE_NONE;
				rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
				rasterizationState.depthClampEnable = VK_FALSE;
				rasterizationState.lineWidth = 1.0f;

				VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};
				{
					blendAttachmentState[0].colorWriteMask = 0xf;
					blendAttachmentState[0].blendEnable = VK_FALSE;
				}

				VkPipelineColorBlendStateCreateInfo colorBlendState = {};
				colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlendState.attachmentCount = 1;
				colorBlendState.pAttachments = blendAttachmentState;

				VkPipelineViewportStateCreateInfo viewportState = {};
				viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				viewportState.viewportCount = 1;
				viewportState.scissorCount = 1;

				std::vector<VkDynamicState> dynamicStateEnables;
				dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
				dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);

				VkPipelineDynamicStateCreateInfo dynamicState = {};
				dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				dynamicState.pDynamicStates = dynamicStateEnables.data();
				dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

				VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
				depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				depthStencilState.depthTestEnable = VK_FALSE;
				depthStencilState.depthWriteEnable = VK_FALSE;
				depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
				depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

				VkPipelineMultisampleStateCreateInfo multisampleState = {};
				multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

				BufferLayout layout(
					{
						{ ShaderDataType::Float3, "a_Position" }
					});

				struct VertexData
				{
					glm::vec3 pos;
				};

				VkVertexInputBindingDescription vertexInputBinding = {};
				vertexInputBinding.binding = 0;
				vertexInputBinding.stride = sizeof(VertexData);
				vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				std::vector<VkVertexInputAttributeDescription> vertexInputAttributs(layout.GetElements().size());
				{
					uint32_t index = 0;
					for (const auto& element : layout.GetElements())
					{
						vertexInputAttributs[index].binding = 0;
						vertexInputAttributs[index].location = index;
						vertexInputAttributs[index].format = VK_FORMAT_R32G32B32_SFLOAT; // TODO: add more formats!
						vertexInputAttributs[index].offset = element.offset;
						index++;
					}
				}

				// Vertex input state used for pipeline creation
				VkPipelineVertexInputStateCreateInfo vertexInputState = {};
				vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vertexInputState.vertexBindingDescriptionCount = 1;
				vertexInputState.pVertexBindingDescriptions = &vertexInputBinding;
				vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributs.size());
				vertexInputState.pVertexAttributeDescriptions = vertexInputAttributs.data();

				pipelineCreateInfo.stageCount = static_cast<uint32_t>(shader.GetVkPipelineShaderStages().size());
				pipelineCreateInfo.pStages = shader.GetVkPipelineShaderStages().data();

				// Assign the pipeline states to the pipeline creation info structure

				pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipelineCreateInfo.layout = pipelinelayout;
				pipelineCreateInfo.pVertexInputState = &vertexInputState;
				pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
				pipelineCreateInfo.pRasterizationState = &rasterizationState;
				pipelineCreateInfo.pColorBlendState = &colorBlendState;
				pipelineCreateInfo.pMultisampleState = &multisampleState;
				pipelineCreateInfo.pViewportState = &viewportState;
				pipelineCreateInfo.pDepthStencilState = &depthStencilState;
				pipelineCreateInfo.renderPass = renderpass;
				pipelineCreateInfo.pDynamicState = &dynamicState;

				VkPipelineCacheCreateInfo pipelineCacheCI = {};
				{
					pipelineCacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
				}

				VK_CHECK_RESULT(vkCreatePipelineCache(device, &pipelineCacheCI, nullptr, &cache));
				VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, cache, 1, &pipelineCreateInfo, nullptr, &pipeline));
			}

			// Render

			VkClearValue clearValues[1];
			clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 0.0f } };

			VkRenderPassBeginInfo renderPassBeginInfo = {};
			{
				renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassBeginInfo.renderPass = renderpass;
				renderPassBeginInfo.framebuffer = offscreen.framebuffer;
				renderPassBeginInfo.renderArea.extent.width = dim;
				renderPassBeginInfo.renderArea.extent.height = dim;
				renderPassBeginInfo.clearValueCount = 1;
				renderPassBeginInfo.pClearValues = clearValues;
			}

			std::vector<glm::mat4> matrices = {
				// POSITIVE_X
				glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
				// NEGATIVE_X
				glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
				// POSITIVE_Y
				glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
				// NEGATIVE_Y
				glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
				// POSITIVE_Z
				glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
				// NEGATIVE_Z
				glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			};

			float skyboxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};

			Ref<VertexBuffer> skyBoxFB = VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices));

			VkCommandBuffer cmdBuffer = VulkanCommandBuffer::CreateSingleCommandBuffer();
			{
				VkViewport viewport = {};
				{
					viewport.width = (float)dim;
					viewport.height = (float)dim;
					viewport.minDepth = 0.0f;
					viewport.maxDepth = 1.0f;
				}

				VkRect2D rect2D = {};
				{
					rect2D.extent.width = dim;
					rect2D.extent.height = dim;
					rect2D.offset.x = 0;
					rect2D.offset.y = 0;
				}

				vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
				vkCmdSetScissor(cmdBuffer, 0, 1, &rect2D);

				VkImageSubresourceRange subresourceRange = {};
				subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				subresourceRange.baseMipLevel = 0;
				subresourceRange.levelCount = numMips;
				subresourceRange.layerCount = 6;

				VulkanTexture::SetImageLayout(cmdBuffer,
					outImage,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					subresourceRange);

				for (uint32_t m = 0; m < numMips; m++)
				{
					pushBlock.roughness = (float)m / (float)(numMips - 1);
					for (uint32_t f = 0; f < 6; f++)
					{
						viewport.width = static_cast<float>(dim * std::pow(0.5f, m));
						viewport.height = static_cast<float>(dim * std::pow(0.5f, m));
						vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

						// Render scene from cube face's point of view
						vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

						// Update shader push constant block
						pushBlock.mvp = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];

						vkCmdPushConstants(cmdBuffer, pipelinelayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushBlock), &pushBlock);

						vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
						vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelinelayout, 0, 1, &descriptorset, 0, NULL);

						VkDeviceSize offsets[1] = { 0 };
						vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &skyBoxFB->GetVulkanVertexBuffer().GetBuffer(), offsets);

						vkCmdDraw(cmdBuffer, 36, 1, 0, 0);

						vkCmdEndRenderPass(cmdBuffer);

						VulkanTexture::SetImageLayout(cmdBuffer,
							offscreen.image,
							VK_IMAGE_ASPECT_COLOR_BIT,
							VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
							VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

						// Copy region for transfer from framebuffer to cube face
						VkImageCopy copyRegion = {};

						copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						copyRegion.srcSubresource.baseArrayLayer = 0;
						copyRegion.srcSubresource.mipLevel = 0;
						copyRegion.srcSubresource.layerCount = 1;
						copyRegion.srcOffset = { 0, 0, 0 };

						copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						copyRegion.dstSubresource.baseArrayLayer = f;
						copyRegion.dstSubresource.mipLevel = m;
						copyRegion.dstSubresource.layerCount = 1;
						copyRegion.dstOffset = { 0, 0, 0 };

						copyRegion.extent.width = static_cast<uint32_t>(viewport.width);
						copyRegion.extent.height = static_cast<uint32_t>(viewport.height);
						copyRegion.extent.depth = 1;

						vkCmdCopyImage(
							cmdBuffer,
							offscreen.image,
							VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
							outImage,
							VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							1,
							&copyRegion);

						VulkanTexture::SetImageLayout(cmdBuffer,
							offscreen.image,
							VK_IMAGE_ASPECT_COLOR_BIT,
							VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
							VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
					}
				}

				VulkanTexture::SetImageLayout(cmdBuffer,
					outImage,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					subresourceRange);
			}
			VulkanCommandBuffer::FlushCommandBuffer(cmdBuffer);

			outImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			outImageInfo.imageView = outImageView;
			outImageInfo.sampler = outSampler;

			vkDestroyRenderPass(device, renderpass, nullptr);
			vkDestroyFramebuffer(device, offscreen.framebuffer, nullptr);
			vkFreeMemory(device, offscreen.memory, nullptr);
			vkDestroyImageView(device, offscreen.view, nullptr);
			vkDestroyImage(device, offscreen.image, nullptr);
			vkDestroyDescriptorPool(device, descriptorpool, nullptr);
			vkDestroyDescriptorSetLayout(device, descriptorsetlayout, nullptr);
			vkDestroyPipeline(device, pipeline, nullptr);
			vkDestroyPipelineLayout(device, pipelinelayout, nullptr);
		}


		auto end = std::chrono::high_resolution_clock::now();
		double diff = std::chrono::duration<double, std::milli>(end - start).count();

		NATIVE_INFO("Generating pre-filtered enivornment cube with {} mip levels took {} ms", numMips, diff);
	}

	void VulkanPBR::DestroyAttachment(const PBRAttachment& obj)
	{
		auto device = VulkanContext::GetDevice().GetLogicalDevice();

		if (obj.Image != VK_NULL_HANDLE)
			vkDestroyImage(device, obj.Image, nullptr);

		if (obj.ImageView != VK_NULL_HANDLE)
			vkDestroyImageView(device, obj.ImageView, nullptr);

		if (obj.Sampler != VK_NULL_HANDLE)
			vkDestroySampler(device, obj.Sampler, nullptr);
		
		if (obj.DeviceMemory != VK_NULL_HANDLE)
			vkFreeMemory(device, obj.DeviceMemory, nullptr);
	}

	void VulkanPBR::Init(const std::string& cubeMapFile, TextureFormat format)
	{
		m_SkyBox = new VulkanTexture();
		m_SkyBox->LoadCubeMap(cubeMapFile, format);

		GenerateBRDFLUT(m_BRDFLUT.Image, m_BRDFLUT.ImageView, m_BRDFLUT.Sampler, m_BRDFLUT.DeviceMemory, m_BRDFLUTImageInfo);

		GenerateIrradianceCube(m_Irradiance.Image, m_Irradiance.ImageView, m_Irradiance.Sampler,
			m_Irradiance.DeviceMemory, m_SkyBox, m_IrradianceImageInfo);

		GeneratePrefilteredCube(m_PrefilteredCube.Image, m_PrefilteredCube.ImageView,
			m_PrefilteredCube.Sampler, m_PrefilteredCube.DeviceMemory, m_SkyBox, m_PrefilteredCubeImageInfo);
	}

	void VulkanPBR::Reload(const std::string& cubeMapFile, TextureFormat format)
	{
		Free();
		Init(cubeMapFile, format);
	}

	void VulkanPBR::Free()
	{
		delete m_SkyBox;
		m_SkyBox = nullptr;

		DestroyAttachment(m_BRDFLUT);
		DestroyAttachment(m_Irradiance);
		DestroyAttachment(m_PrefilteredCube);
	}

	const VkDescriptorImageInfo& VulkanPBR::GetBRDFLUTImageInfo()
	{
		return m_BRDFLUTImageInfo;
	}

	const VkDescriptorImageInfo& VulkanPBR::GetIrradianceImageInfo()
	{
		return m_IrradianceImageInfo;
	}

	const VkDescriptorImageInfo& VulkanPBR::GetPrefilteredCubeImageInfo()
	{
		return m_PrefilteredCubeImageInfo;
	}

	const VulkanTexture&  VulkanPBR::GetSkyBox()
	{
		return *m_SkyBox;
	}
}
