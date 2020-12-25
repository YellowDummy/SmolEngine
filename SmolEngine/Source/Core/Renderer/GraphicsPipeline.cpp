#include "stdafx.h"
#include "GraphicsPipeline.h"

#ifndef SMOLENGINE_OPENGL_IMPL
#include "Core/Renderer/Vulkan/VulkanContext.h"
#include "Core/Renderer/Vulkan/VulkanTexture.h"
#endif

#include "Core/Renderer/RendererAPI.h"
#include "Core/Renderer/Framebuffer.h"

namespace SmolEngine
{
	bool GraphicsPipeline::Create(const GraphicsPipelineCreateInfo* pipelineInfo)
	{
		if (!IsPipelineCreateInfoValid(pipelineInfo))
		{
			return false;
		}

		if (pipelineInfo->ShaderCreateInfo->UseSingleFile)
		{
			m_Shader = Shader::Create(pipelineInfo->ShaderCreateInfo->SingleFilePath);
		}
		else
		{
			const auto& vexrtex = pipelineInfo->ShaderCreateInfo->FilePaths[ShaderType::Vertex];
			const auto& frag = pipelineInfo->ShaderCreateInfo->FilePaths[ShaderType::Fragment];
			const auto& compute = pipelineInfo->ShaderCreateInfo->FilePaths[ShaderType::Compute];

			m_Shader = Shader::Create(vexrtex, frag, pipelineInfo->ShaderCreateInfo->Optimize, compute);
		}

		m_VertexBuffers.resize(pipelineInfo->VertexBuffer->BuffersCount);
		for (uint32_t i = 0; i < pipelineInfo->VertexBuffer->BuffersCount; ++i)
		{
			pipelineInfo->VertexBuffer->IsAllocateMemOnly ?
				m_VertexBuffers[i] = VertexBuffer::Create(pipelineInfo->VertexBuffer->Size) :
				m_VertexBuffers[i] = VertexBuffer::Create(pipelineInfo->VertexBuffer->Vertices, pipelineInfo->VertexBuffer->Size);
		}

		m_IndexBuffers.resize(pipelineInfo->IndexBuffer->BuffersCount);
		for (uint32_t i = 0; i < pipelineInfo->IndexBuffer->BuffersCount; ++i)
		{
			m_IndexBuffers[i] = IndexBuffer::Create(pipelineInfo->IndexBuffer->Indices, (pipelineInfo->IndexBuffer->IndicesCount));
		}


#ifdef SMOLENGINE_OPENGL_IMPL

		for (uint32_t i = 0; i < m_VertexBuffers.size(); ++i)
		{
			m_VertexBuffers[i]->SetLayout(*pipelineInfo->VertexBuffer->BufferLayot);
		}

		m_VextexArray = VertexArray::Create();
		m_VextexArray->SetVertexBuffer(m_VertexBuffers[0]);
		m_VextexArray->SetIndexBuffer(m_IndexBuffers[0]);
#else
		m_VulkanPipeline = {};
		std::vector<VulkanTexture*> textures(pipelineInfo->ShaderCreateInfo->Textures.size());
		{
			uint32_t index = 0;
			for (const auto& texture : pipelineInfo->ShaderCreateInfo->Textures)
			{
				textures[index] = texture->GetVulkanTexture();
				index++;
			}
		}

		VulkanPipelineSpecification pipelineSpecCI = {};
		{
			pipelineSpecCI.Device = &VulkanContext::GetDevice();
			pipelineSpecCI.Shader = m_Shader->GetVulkanShader();
			pipelineSpecCI.TargetSwapchain = &VulkanContext::GetSwapchain();
			pipelineSpecCI.BufferLayout = pipelineInfo->VertexBuffer->BufferLayot;
			pipelineSpecCI.Shader = m_Shader->GetVulkanShader();
			pipelineSpecCI.Textures = std::move(textures);
			pipelineSpecCI.Stride = pipelineInfo->VertexBuffer->Stride;
			pipelineSpecCI.IsAlphaBlendingEnabled = pipelineInfo->IsAlphaBlendingEnabled;
			pipelineSpecCI.DescriptorSets = pipelineInfo->DescriptorSets;
		}

		m_VulkanPipeline.Invalidate(&pipelineSpecCI);
#endif // SMOLENGINE_OPENGL_IMPL

		return true;
	}

	void GraphicsPipeline::BeginRenderPass(Ref<Framebuffer> framebuffer, const glm::vec4& clearColors)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		m_Shader->Bind();
		m_VextexArray->Bind();
#else
		auto& offscreenPass = framebuffer->GetVulkanFramebuffer().GetOffscreenPass();
		auto& scpec = framebuffer->GetSpecification();

		VkClearValue clearValues[2];
		//clearValues[0].color = { { clearColors.r,  clearColors.g,  clearColors.b,  clearColors.a } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		{
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = offscreenPass.renderPass;
			renderPassBeginInfo.framebuffer = offscreenPass.frameBuffer;
			renderPassBeginInfo.renderArea.extent.width = scpec.Width;
			renderPassBeginInfo.renderArea.extent.height = scpec.Height;
			renderPassBeginInfo.clearValueCount = 2;
			renderPassBeginInfo.pClearValues = clearValues;
		}

		vkCmdBeginRenderPass(m_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Update dynamic viewport state
		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = (float)scpec.Height;
		viewport.height = -(float)scpec.Height;
		viewport.width = (float)scpec.Width;
		viewport.minDepth = (float)0.0f;
		viewport.maxDepth = (float)1.0f;
		vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);

		// Update dynamic scissor state
		VkRect2D scissor = {};
		scissor.extent.width = scpec.Width;
		scissor.extent.height = scpec.Height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);

#endif // SMOLENGINE_OPENGL_IMPL
	}

	void GraphicsPipeline::EndRenderPass()
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		m_Shader->UnBind();
		m_VextexArray->UnBind();
#else
		vkCmdEndRenderPass(m_CommandBuffer);
#endif
	}

	void GraphicsPipeline::ClearColors(Ref<Framebuffer>& framebuffer)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

#else
		VkClearRect clearRect = {};
		clearRect.layerCount = 1;
		clearRect.baseArrayLayer = 0;
		clearRect.rect.offset = { 0, 0 };
		clearRect.rect.extent = { (uint32_t)framebuffer->GetSpecification().Width, (uint32_t)framebuffer->GetSpecification().Height };

		vkCmdClearAttachments(m_CommandBuffer, 2, framebuffer->GetVulkanFramebuffer().m_OffscreenPass.clearAttachments, 1, &clearRect);
#endif
	}

	void GraphicsPipeline::BeginBufferSubmit()
	{
#ifndef SMOLENGINE_OPENGL_IMPL

		for (auto& vertextBuffer : m_VertexBuffers)
		{
			vertextBuffer->GetVulkanVertexBuffer().MapMemory();
		}

		for (auto& indexBuffer : m_IndexBuffers)
		{
			indexBuffer->GetVulkanIndexBuffer().MapMemory();
		}

		for (auto& [binding, ubo] : m_Shader->GetVulkanShader()->m_UniformBuffers)
		{
			ubo.VkBuffer.MapMemory();
		}
#endif
	}

	void GraphicsPipeline::EndBufferSubmit()
	{
#ifndef SMOLENGINE_OPENGL_IMPL

		for (auto& vertextBuffer : m_VertexBuffers)
		{
			vertextBuffer->GetVulkanVertexBuffer().UnMapMemory();
		}

		for (auto& indexBuffer : m_IndexBuffers)
		{
			indexBuffer->GetVulkanIndexBuffer().UnMapMemory();
		}

		for (auto& [binding, ubo] : m_Shader->GetVulkanShader()->m_UniformBuffers)
		{
			ubo.VkBuffer.UnMapMemory();
		}
#endif
	}

	void GraphicsPipeline::BeginCommandBuffer()
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		m_CommandBuffer = VulkanCommandBuffer::CreateSingleCommandBuffer();
#endif
	}

	void GraphicsPipeline::EndCommandBuffer()
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		VulkanCommandBuffer::FlushCommandBuffer(m_CommandBuffer);
#endif
	}

	void GraphicsPipeline::FlushCommandBuffer()
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		VulkanCommandBuffer::FlushCommandBuffer(m_CommandBuffer);
#endif
	}

	void GraphicsPipeline::DrawIndexed(uint32_t vertexBufferIndex, uint32_t indexBufferIndex, uint32_t descriptorSetIndex)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		if (vertexBufferIndex > 0)
		{
			m_VextexArray->SetVertexBuffer(m_VertexBuffers[vertexBufferIndex]);
		}
		if (indexBufferIndex > 0)
		{
			m_VextexArray->SetIndexBuffer(m_IndexBuffers[indexBufferIndex]);
		}

		RendererCommand::DrawIndexed(m_VextexArray);
#else
		// Bind the rendering pipeline
		// The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time

		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline.GetVkPipeline());

		// Bind Vertex Buffer
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &m_VertexBuffers[vertexBufferIndex]->GetVulkanVertexBuffer().GetBuffer(), offsets);

		// Bind Index Buffer
		vkCmdBindIndexBuffer(m_CommandBuffer, m_IndexBuffers[indexBufferIndex]->GetVulkanIndexBuffer().GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// Bind descriptor sets describing shader binding points
		const auto& descriptorSet = m_VulkanPipeline.GetVkDescriptorSet(descriptorSetIndex);
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline.GetVkPipelineLayot(), 0, 1,
			&descriptorSet, 0, nullptr);

		// Draw indexed
		vkCmdDrawIndexed(m_CommandBuffer, m_IndexBuffers[indexBufferIndex]->GetVulkanIndexBuffer().GetCount(), 1, 0, 0, 1);
	
#endif
	}

	void GraphicsPipeline::SumbitUniformBuffer(uint32_t bindingPoint, size_t size, const void* data, uint32_t offset)
	{
		m_Shader->SumbitUniformBuffer(bindingPoint, data, size, offset);
	}

	void GraphicsPipeline::SumbitPushConstant(ShaderType shaderStage, size_t size, const void* data)
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		vkCmdPushConstants(m_CommandBuffer, m_VulkanPipeline.GetVkPipelineLayot(), VulkanShader::GetVkShaderStage(shaderStage), 0, size, data);
#endif
	}

	void GraphicsPipeline::UpdateVertextBuffer(void* vertices, size_t size, uint32_t offset, uint32_t bufferIndex)
	{
		m_VertexBuffers[bufferIndex]->UploadData(vertices, size, offset);
#ifdef SMOLENGINE_OPENGL_IMPL
		m_VextexArray->SetVertexBuffer(m_VertexBuffers[bufferIndex]);
#endif
	}

	void GraphicsPipeline::UpdateIndexBuffer(uint32_t* indices, size_t count, uint32_t bufferIndex)
	{
		m_IndexBuffers[bufferIndex]->UploadData(indices, count);

#ifdef SMOLENGINE_OPENGL_IMPL
		m_VextexArray->SetIndexBuffer(m_IndexBuffers[bufferIndex]);
#endif
	}

	void GraphicsPipeline::Update2DTextures(const std::vector<Ref<Texture2D>>& textures, uint32_t descriptorSetIndex)
	{
		uint32_t index = 0;
#ifndef SMOLENGINE_OPENGL_IMPL
		std::vector<VulkanTexture*> vkTextures(textures.size());
#endif
		for (const auto& tex : textures)
		{
			if (tex == nullptr)
			{
				break;
			}

#ifdef SMOLENGINE_OPENGL_IMPL
			tex->Bind(index);
#else
			vkTextures[index] = tex->GetVulkanTexture();
#endif
			index++;
		}

#ifndef SMOLENGINE_OPENGL_IMPL
		m_VulkanPipeline.UpdateSamplers2D(vkTextures, m_CommandBuffer, descriptorSetIndex);
#endif
	}

	bool GraphicsPipeline::IsPipelineCreateInfoValid(const GraphicsPipelineCreateInfo* pipelineInfo)
	{
		if (!pipelineInfo->IndexBuffer->Indices || pipelineInfo->IndexBuffer->BuffersCount == 0 || pipelineInfo->VertexBuffer->Size == 0 || !pipelineInfo->VertexBuffer->BufferLayot ||
			!pipelineInfo->ShaderCreateInfo || pipelineInfo->VertexBuffer->Stride == 0 || pipelineInfo->VertexBuffer->BuffersCount == 0)
		{
			return false;
		}

		if (!pipelineInfo->VertexBuffer->Vertices && pipelineInfo->VertexBuffer->IsAllocateMemOnly == false)
		{
			return false;
		}

		if (pipelineInfo->ShaderCreateInfo->SingleFilePath.empty() && pipelineInfo->ShaderCreateInfo->UseSingleFile)
		{
			return false;
		}

		return true;
	}
}