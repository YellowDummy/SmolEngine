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
		if (!pipelineInfo->IndexBuffer->Indices || pipelineInfo->IndexBuffer->Count == 0 ||
			!pipelineInfo->VertexBuffer->Vertices || pipelineInfo->VertexBuffer->Size == 0 || !pipelineInfo->VertexBuffer->BufferLayot ||
			!pipelineInfo->ShaderCreateInfo)
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

		m_IndexBuffer = IndexBuffer::Create(pipelineInfo->IndexBuffer->Indices, pipelineInfo->IndexBuffer->Count);
		m_VertexBuffer = VertexBuffer::Create(pipelineInfo->VertexBuffer->Vertices, pipelineInfo->VertexBuffer->Size);
		m_VextexArray = VertexArray::Create();

#ifdef SMOLENGINE_OPENGL_IMPL

		m_VertexBuffer->SetLayout(*vertexCI->BufferLayot);
		m_VextexArray->SetVertexBuffer(m_VertexBuffer);
		m_VextexArray->SetIndexBuffer(m_IndexBuffer);
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
		}

		m_VulkanPipeline.Invalidate(&pipelineSpecCI);

#endif // SMOLENGINE_OPENGL_IMPL

	}

	void GraphicsPipeline::BeginRenderPass(Ref<Framebuffer>& framebuffer, const glm::vec4& clearColors)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_Shader->Bind();
		m_VextexArray->Bind();
#else
		m_CommandBuffer = VulkanContext::GetCommandBuffer().GetVkCommandBuffer();
		auto& offscreenPass = framebuffer->GetVulkanFramebuffer().GetOffscreenPass();
		auto& scpec = framebuffer->GetSpecification();

		VkClearValue clearValues[2];
		clearValues[0].color = { { clearColors.r,  clearColors.g,  clearColors.b,  clearColors.a } };
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
		m_CommandBuffer = VK_NULL_HANDLE;
#endif
	}

	void GraphicsPipeline::DrawIndexed()
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		RendererCommand::DrawIndexed(m_VextexArray);

#else
		const auto& descriptorsSet = m_VulkanPipeline.GetVkDescriptorSet();

		// Bind descriptor sets describing shader binding points
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline.GetVkPipelineLayot(), 0, 1,
			&descriptorsSet, 0, nullptr);

		// Bind the rendering pipeline
		// The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time

		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline.GetVkPipeline());

		// Bind Vertex Buffer
		VkDeviceSize offsets[1] = { 0 }; //temp
		vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &m_VertexBuffer->GetVulkanVertexBuffer().GetBuffer(), offsets);

		// Bind Index Buffer
		vkCmdBindIndexBuffer(m_CommandBuffer, m_IndexBuffer->GetVulkanIndexBuffer().GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// Draw indexed triangle
		vkCmdDrawIndexed(m_CommandBuffer, m_IndexBuffer->GetVulkanIndexBuffer().GetCount(), 1, 0, 0, 0);
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

	void GraphicsPipeline::UpdateVertextBuffer(float* vertices, size_t size, uint32_t offset)
	{
		m_VertexBuffer->UploadData(vertices, size, offset);

#ifdef SMOLENGINE_OPENGL_IMPL
		m_VextexArray->SetVertexBuffer(m_VertexBuffer);
#endif
	}

	void GraphicsPipeline::UpdateIndexBuffer(uint32_t* indices, size_t count)
	{
		m_IndexBuffer->Destory();
		m_IndexBuffer = IndexBuffer::Create(indices, count);

#ifdef SMOLENGINE_OPENGL_IMPL
		m_VextexArray->SetIndexBuffer(m_IndexBuffer);
#endif
	}

	void GraphicsPipeline::Update2DTextures(const std::vector<Ref<Texture2D>>& textures)
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		std::vector<VulkanTexture*> vkTextures(textures.size());
		uint32_t index = 0;
#endif
		for (const auto& tex: textures)
		{
#ifdef SMOLENGINE_OPENGL_IMPL
			tex->Bind();
#else
			vkTextures[index] = tex->GetVulkanTexture();
			index++;
#endif
		}

#ifndef SMOLENGINE_OPENGL_IMPL
		m_VulkanPipeline.UpdateSamplers2D(vkTextures);
#endif
	}
}