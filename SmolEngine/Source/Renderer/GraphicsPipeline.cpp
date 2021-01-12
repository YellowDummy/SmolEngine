#include "stdafx.h"
#include "GraphicsPipeline.h"

#ifndef SMOLENGINE_OPENGL_IMPL
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanTexture.h"
#endif

#include "Renderer/RendererAPI.h"
#include "Renderer/Framebuffer.h"

namespace SmolEngine
{
	GraphicsPipeline::~GraphicsPipeline()
	{
		Destroy();
	}

	bool GraphicsPipeline::Create(const GraphicsPipelineCreateInfo* pipelineInfo)
	{
		if (!IsPipelineCreateInfoValid(pipelineInfo))
		{
			NATIVE_ERROR("GraphicsPipeline was not created!");
			assert(IsPipelineCreateInfoValid(pipelineInfo) == true);
			return false;
		}

		m_Shader = std::make_shared<Shader>();
		if (pipelineInfo->ShaderCreateInfo->UseSingleFile)
		{
			Shader::Create(m_Shader, pipelineInfo->ShaderCreateInfo->SingleFilePath);
		}
		else
		{
			const auto& vexrtex = pipelineInfo->ShaderCreateInfo->FilePaths[ShaderType::Vertex];
			const auto& frag = pipelineInfo->ShaderCreateInfo->FilePaths[ShaderType::Fragment];
			const auto& compute = pipelineInfo->ShaderCreateInfo->FilePaths[ShaderType::Compute];

			Shader::Create(m_Shader, vexrtex, frag, pipelineInfo->ShaderCreateInfo->Optimize, compute);
		}

		uint32_t vertexBuffersCount = pipelineInfo->VertexBuffer->BuffersCount;
		m_VertexBuffers.resize(vertexBuffersCount);
		if (pipelineInfo->VertexBuffer->Vertices.size() == vertexBuffersCount && pipelineInfo->VertexBuffer->Sizes.size() == vertexBuffersCount)
		{
			for (uint32_t i = 0; i < vertexBuffersCount; ++i)
			{
				pipelineInfo->VertexBuffer->IsAllocateMemOnly ?
					m_VertexBuffers[i] = VertexBuffer::Create(pipelineInfo->VertexBuffer->Sizes[i]) :
					m_VertexBuffers[i] = VertexBuffer::Create(pipelineInfo->VertexBuffer->Vertices[i], pipelineInfo->VertexBuffer->Sizes[i]);
			}
		}
		else
		{
			for (uint32_t i = 0; i < vertexBuffersCount; ++i)
			{
				pipelineInfo->VertexBuffer->IsAllocateMemOnly ?
					m_VertexBuffers[i] = VertexBuffer::Create(pipelineInfo->VertexBuffer->Sizes[0]) :
					m_VertexBuffers[i] = VertexBuffer::Create(pipelineInfo->VertexBuffer->Vertices[0], pipelineInfo->VertexBuffer->Sizes[0]);
			}
		}

		uint32_t indexBuffersCount = pipelineInfo->IndexBuffer->BuffersCount;
		m_IndexBuffers.resize(indexBuffersCount);
		if (pipelineInfo->IndexBuffer->Indices.size() == indexBuffersCount && pipelineInfo->IndexBuffer->IndicesCounts.size() == indexBuffersCount)
		{
			for (uint32_t i = 0; i < indexBuffersCount; ++i)
			{
				m_IndexBuffers[i] = IndexBuffer::Create(pipelineInfo->IndexBuffer->Indices[i], pipelineInfo->IndexBuffer->IndicesCounts[i]);
			}
		}
		else
		{
			for (uint32_t i = 0; i < indexBuffersCount; ++i)
			{
				m_IndexBuffers[i] = IndexBuffer::Create(pipelineInfo->IndexBuffer->Indices[0], pipelineInfo->IndexBuffer->IndicesCounts[0]);
			}
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
			pipelineSpecCI.BufferLayout = *pipelineInfo->VertexBuffer->BufferLayot;
			pipelineSpecCI.Shader = m_Shader->GetVulkanShader();
			pipelineSpecCI.Textures = std::move(textures);
			pipelineSpecCI.Stride = pipelineInfo->VertexBuffer->Stride;
			pipelineSpecCI.IsAlphaBlendingEnabled = pipelineInfo->IsAlphaBlendingEnabled;
			pipelineSpecCI.DescriptorSets = pipelineInfo->DescriptorSets;
			pipelineSpecCI.Name = pipelineInfo->PipelineName;
			pipelineSpecCI.PipelineDrawModes = pipelineInfo->PipelineDrawModes;
		}

		m_VulkanPipeline.Invalidate(pipelineSpecCI);
		for (DrawMode mode : pipelineInfo->PipelineDrawModes)
		{
			m_VulkanPipeline.CreatePipeline(mode);
		}
		m_Shader->GetVulkanShader()->DeleteShaderModules();
#endif
		m_State.DescriptorSets = pipelineInfo->DescriptorSets;
		m_State.IsAlphaBlendingEnabled = pipelineInfo->IsAlphaBlendingEnabled;
		m_State.Layout = *pipelineInfo->VertexBuffer->BufferLayot;
		m_State.Stride = pipelineInfo->VertexBuffer->Stride;
		m_State.PipelineName = pipelineInfo->PipelineName;
		return true;
	}

	bool GraphicsPipeline::Reload()
	{
		bool shader_res = m_Shader->Realod();
		bool pipe_res = m_VulkanPipeline.ReCreate();
#ifndef SMOLENGINE_OPENGL_IMPL
		m_Shader->GetVulkanShader()->DeleteShaderModules();
#endif

		return true;
	}

	void GraphicsPipeline::Destroy()
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		for (auto& index : m_IndexBuffers)
		{
			index->GetVulkanIndexBuffer().Destroy();
		}

		for (auto& vertex : m_VertexBuffers)
		{
			vertex->GetVulkanVertexBuffer().Destroy();
		}
#endif
		m_VertexBuffers.clear();
		m_IndexBuffers.clear();

#ifndef SMOLENGINE_OPENGL_IMPL
		m_VulkanPipeline.Destroy();
		m_VulkanPipeline = {};
#endif
		if (m_Shader)
			m_Shader = nullptr;

		if (m_VextexArray)
			m_VextexArray = nullptr;
	}

	void GraphicsPipeline::BeginRenderPass(Ref<Framebuffer> framebuffer)
	{
		m_RenderpassFramebuffer = framebuffer;
#ifdef SMOLENGINE_OPENGL_IMPL

		m_Shader->Bind();
		m_VextexArray->Bind();
		m_RenderpassFramebuffer->Bind();
#else
		auto& offscreenPass = framebuffer->GetVulkanFramebuffer().GetOffscreenPass();

		VkClearValue clearValues[2];
		clearValues[1].depthStencil = { 1.0f, 0 };

		uint32_t width = 0;
		uint32_t height = 0;

		VkRenderPass selectedPass = nullptr;
		VkFramebuffer selectedFramebuffer = nullptr;
		if (framebuffer)
		{
			auto& scpec = framebuffer->GetSpecification();
			selectedPass = offscreenPass.renderPass;
			selectedFramebuffer = offscreenPass.frameBuffer;
			width = scpec.Width;
			height = scpec.Height;
		}
		else
		{
			auto& framebuffers = VulkanContext::GetSwapchain().GetSwapchainFramebuffer().GetVkFramebuffers();
			uint32_t index = VulkanContext::GetSwapchain().GetCurrentBufferIndex();

			clearValues[0].color = { { 0.1f, 0.1f, 0.1f, 1.0f } };
			clearValues[1].depthStencil = { 1.0f, 0 };

			width = VulkanContext::GetSwapchain().GetWidth();
			height = VulkanContext::GetSwapchain().GetHeight();

			selectedPass = VulkanContext::GetSwapchain().GetRenderPass();
			selectedFramebuffer = framebuffers[index];
		}

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		{
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = selectedPass;
			renderPassBeginInfo.framebuffer = selectedFramebuffer;
			renderPassBeginInfo.renderArea.extent.width = width;
			renderPassBeginInfo.renderArea.extent.height = height;
			renderPassBeginInfo.clearValueCount = 2;
			renderPassBeginInfo.pClearValues = clearValues;
		}

		vkCmdBeginRenderPass(m_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Update dynamic viewport state
		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = (float)height;
		viewport.height = -(float)height;
		viewport.width = (float)width;
		viewport.minDepth = (float)0.0f;
		viewport.maxDepth = (float)1.0f;
		vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);

		// Update dynamic scissor state
		VkRect2D scissor = {};
		scissor.extent.width = width;
		scissor.extent.height = height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);

#endif // SMOLENGINE_OPENGL_IMPL
	}

	void GraphicsPipeline::EndRenderPass()
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		m_RenderpassFramebuffer->UnBind();
		m_Shader->UnBind();
		m_VextexArray->UnBind();
#else
		m_RenderpassFramebuffer = nullptr;
		vkCmdEndRenderPass(m_CommandBuffer);
#endif
	}

	void GraphicsPipeline::ClearColors(const glm::vec4& clearColors)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		RendererCommand::SetClearColor(clearColors);
		RendererCommand::Clear();
#else
		if (!m_RenderpassFramebuffer)
			return;

		VkClearRect clearRect = {};
		clearRect.layerCount = 1;
		clearRect.baseArrayLayer = 0;
		clearRect.rect.offset = { 0, 0 };
		clearRect.rect.extent = { (uint32_t)m_RenderpassFramebuffer->GetSpecification().Width, 
			(uint32_t)m_RenderpassFramebuffer->GetSpecification().Height };

		auto& vkFrameBuffer = m_RenderpassFramebuffer->GetVulkanFramebuffer();
		vkFrameBuffer.SetClearColors(clearColors);
		vkCmdClearAttachments(m_CommandBuffer, 2, vkFrameBuffer.m_OffscreenPass.clearAttachments, 1, &clearRect);
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

	void GraphicsPipeline::BeginCommandBuffer(bool isSwapchainTarget)
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		if (isSwapchainTarget)
		{
			m_CommandBuffer = VulkanContext::GetCurrentVkCmdBuffer();
			m_IsSwapchainTarget = true;
			return;
		}

		m_CommandBuffer = VulkanCommandBuffer::CreateSingleCommandBuffer();
#else
		RendererCommand::Reset();
#endif
	}

	void GraphicsPipeline::EndCommandBuffer()
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		if (!m_IsSwapchainTarget)
		{
			VulkanCommandBuffer::FlushCommandBuffer(m_CommandBuffer);
		}
#endif
	}

	void GraphicsPipeline::FlushCommandBuffer()
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		VulkanCommandBuffer::FlushCommandBuffer(m_CommandBuffer);
#endif
	}

	void GraphicsPipeline::DrawIndexed(DrawMode mode, uint32_t vertexBufferIndex, uint32_t indexBufferIndex, uint32_t descriptorSetIndex)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		m_VextexArray->Bind();
		m_VextexArray->SetVertexBuffer(m_VertexBuffers[vertexBufferIndex]);
		m_VextexArray->SetIndexBuffer(m_IndexBuffers[indexBufferIndex]);
		m_Shader->Bind();

		switch (mode)
		{
		case SmolEngine::DrawMode::Triangle:
			RendererCommand::DrawTriangle(m_VextexArray);
			break;
		case SmolEngine::DrawMode::Line:
			RendererCommand::DrawLine(m_VextexArray);
			break;
		case SmolEngine::DrawMode::Fan:
			RendererCommand::DrawFan(m_VextexArray);
			break;
		default:
			break;
		}
#else
		// Bind the rendering pipeline
		// The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time

		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline.GetVkPipeline(mode));

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

	void GraphicsPipeline::Draw(uint32_t vertextCount, DrawMode mode, uint32_t vertexBufferIndex, uint32_t descriptorSetIndex)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		m_VextexArray->Bind();
		m_VextexArray->SetVertexBuffer(m_VertexBuffers[vertexBufferIndex]);
		m_Shader->Bind();

		switch (mode)
		{
		case SmolEngine::DrawMode::Triangle:
			RendererCommand::DrawTriangle(m_VextexArray, 0, vertextCount);
			break;
		case SmolEngine::DrawMode::Line:
			RendererCommand::DrawLine(m_VextexArray, 0, vertextCount);
			break;
		case SmolEngine::DrawMode::Fan:
			RendererCommand::DrawFan(m_VextexArray, 0, vertextCount);
			break;
		default:
			break;
		}
#else
		// Bind the rendering pipeline
		// The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time

		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline.GetVkPipeline(mode));

		// Bind Vertex Buffer
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &m_VertexBuffers[vertexBufferIndex]->GetVulkanVertexBuffer().GetBuffer(), offsets);

		// Bind descriptor sets describing shader binding points
		const auto& descriptorSet = m_VulkanPipeline.GetVkDescriptorSet(descriptorSetIndex);
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline.GetVkPipelineLayot(), 0, 1,
			&descriptorSet, 0, nullptr);

		// Draw
		vkCmdDraw(m_CommandBuffer, vertextCount, 1, 0, 0);
#endif
	}

	void GraphicsPipeline::SumbitUniformBuffer(uint32_t bindingPoint, size_t size, const void* data, uint32_t offset)
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		m_Shader->SumbitUniformBuffer(bindingPoint, data, size, offset);
		//m_VulkanPipeline.UpdateDescriptors();
#endif

	}

	void GraphicsPipeline::SumbitPushConstant(ShaderType shaderStage, size_t size, const void* data)
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		vkCmdPushConstants(m_CommandBuffer, m_VulkanPipeline.GetVkPipelineLayot(), VulkanShader::GetVkShaderStage(shaderStage), 0, size, data);
#endif
	}

	void GraphicsPipeline::UpdateVertextBuffer(void* vertices, size_t size, uint32_t bufferIndex, uint32_t offset)
	{
		m_VertexBuffers[bufferIndex]->UploadData(vertices, size, offset);
#ifdef SMOLENGINE_OPENGL_IMPL
		m_VextexArray->Bind();
		m_VextexArray->SetVertexBuffer(m_VertexBuffers[bufferIndex]);
#endif
	}

	void GraphicsPipeline::UpdateIndexBuffer(uint32_t* indices, size_t count, uint32_t bufferIndex, uint32_t offset)
	{
		m_IndexBuffers[bufferIndex]->UploadData(indices, count);
#ifdef SMOLENGINE_OPENGL_IMPL
		m_VextexArray->Bind();
		m_VextexArray->SetIndexBuffer(m_IndexBuffers[bufferIndex]);
#endif
	}

	void GraphicsPipeline::Update2DTextures(const std::vector<Ref<Texture2D>>& textures, uint32_t descriptorSetIndex)
	{
		m_Shader->Bind();
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
		if (pipelineInfo->IndexBuffer->BuffersCount == 0 || !pipelineInfo->VertexBuffer->BufferLayot ||
			!pipelineInfo->ShaderCreateInfo || pipelineInfo->VertexBuffer->Stride == 0 || pipelineInfo->VertexBuffer->BuffersCount == 0)
			return false;

		if (pipelineInfo->IndexBuffer->Indices.size() != pipelineInfo->IndexBuffer->BuffersCount)
			return false;

		if (pipelineInfo->VertexBuffer->Vertices.size() == 0 && pipelineInfo->VertexBuffer->IsAllocateMemOnly == false)
			return false;

		if (pipelineInfo->ShaderCreateInfo->SingleFilePath.empty() && pipelineInfo->ShaderCreateInfo->UseSingleFile)
			return false;

		if (pipelineInfo->PipelineName.empty())
			return false;

		return true;
	}
}