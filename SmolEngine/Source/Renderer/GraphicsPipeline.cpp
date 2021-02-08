#include "stdafx.h"
#include "GraphicsPipeline.h"

#ifndef SMOLENGINE_OPENGL_IMPL
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanRenderPass.h"
#include "Renderer/Vulkan/VulkanTexture.h"
#endif

#include "Renderer/Framebuffer.h"
#include "Renderer/CubeTexture.h"
#include "Renderer/GraphicsContext.h"

namespace SmolEngine
{
	GraphicsPipeline::~GraphicsPipeline()
	{
		Destroy();
	}

	bool GraphicsPipeline::Create(GraphicsPipelineCreateInfo* pipelineInfo)
	{
		if (pipelineInfo->DescriptorSets < 1 || !pipelineInfo->ShaderCreateInfo)
			return false;

		m_GraphicsContext = GraphicsContext::GetSingleton();
		m_Shader = std::make_shared<Shader>();
		if (pipelineInfo->ShaderCreateInfo->UseSingleFile)
			Shader::Create(m_Shader, pipelineInfo->ShaderCreateInfo->SingleFilePath);
		else
		{
			const auto& vexrtex = pipelineInfo->ShaderCreateInfo->FilePaths[ShaderType::Vertex];
			const auto& frag = pipelineInfo->ShaderCreateInfo->FilePaths[ShaderType::Fragment];
			const auto& compute = pipelineInfo->ShaderCreateInfo->FilePaths[ShaderType::Compute];

			Shader::Create(m_Shader, vexrtex, frag, pipelineInfo->ShaderCreateInfo->Optimize, compute);
		}

#ifdef SMOLENGINE_OPENGL_IMPL
		m_VextexArray = VertexArray::Create();
#else
		m_VulkanPipeline = {};

		if (pipelineInfo->IsUseMSAA && pipelineInfo->IsUseMRT)
			pipelineInfo->IsUseMSAA = false;

		VulkanPipelineSpecification pipelineSpecCI = {};
		{
			pipelineSpecCI.Device = &VulkanContext::GetDevice();
			pipelineSpecCI.Shader = m_Shader->GetVulkanShader();
			pipelineSpecCI.TargetSwapchain = &VulkanContext::GetSwapchain();
			pipelineSpecCI.VertexInputInfos = pipelineInfo->VertexInputInfos;
			pipelineSpecCI.Shader = m_Shader->GetVulkanShader();
			pipelineSpecCI.IsAlphaBlendingEnabled = pipelineInfo->IsAlphaBlendingEnabled;
			pipelineSpecCI.DescriptorSets = pipelineInfo->DescriptorSets;
			pipelineSpecCI.Name = pipelineInfo->PipelineName;
			pipelineSpecCI.PipelineDrawModes = pipelineInfo->PipelineDrawModes;
			pipelineSpecCI.IsTargetsSwapchain = pipelineInfo->IsTargetsSwapchain;
			pipelineSpecCI.IsDepthTestEnabled = pipelineInfo->IsDepthTestEnabled;
			pipelineSpecCI.IsUseMRT = pipelineInfo->IsUseMRT;
			pipelineSpecCI.IsUseMSAA = pipelineInfo->IsUseMSAA;
			pipelineSpecCI.MinDepth = pipelineInfo->MinDepth;
			pipelineSpecCI.MaxDepth = pipelineInfo->MaxDepth;
		}

		if (!m_VulkanPipeline.Invalidate(pipelineSpecCI))
			return false;

		for (DrawMode mode : pipelineInfo->PipelineDrawModes)
		{
			if (!m_VulkanPipeline.CreatePipeline(mode))
				return false;
		}
		m_Shader->GetVulkanShader()->DeleteShaderModules();

#endif
		m_State.DescriptorSets = pipelineInfo->DescriptorSets;
		m_State.IsAlphaBlendingEnabled = pipelineInfo->IsAlphaBlendingEnabled;
		m_State.VertexInputInfos = pipelineInfo->VertexInputInfos;
		m_State.PipelineName = pipelineInfo->PipelineName;
		return true;
	}

	bool GraphicsPipeline::Reload()
	{
		if (!m_Shader->Realod())
			return false;

#ifndef SMOLENGINE_OPENGL_IMPL
		if (!m_VulkanPipeline.ReCreate())
			return false;
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

	void GraphicsPipeline::BeginRenderPass(Ref<Framebuffer>& framebuffer)
	{
		m_RenderpassFramebuffer = framebuffer;
#ifdef SMOLENGINE_OPENGL_IMPL
		m_Shader->Bind();
		m_VextexArray->Bind();
		if(m_RenderpassFramebuffer)
			m_RenderpassFramebuffer->Bind();
#else
		std::vector<VkClearValue> clearValues;
		const FramebufferSpecification& specs = framebuffer->GetSpecification();
		uint32_t width = specs.Width;
		uint32_t height = specs.Height;
		VkRenderPass selectedPass = VulkanRenderPass::GetRenderPass(specs.IsTargetsSwapchain, specs.IsUseMSAA, specs.IsUseMRT);
		VkFramebuffer selectedFramebuffer = framebuffer->GetVulkanFramebuffer().GetCurrentVkFramebuffer();

		if (specs.IsUseMRT)
		{
			clearValues.resize(5);
			clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
			clearValues[1].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
			clearValues[2].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
			clearValues[3].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
			clearValues[4].depthStencil = { 1.0f, 0 };
		}
		else
		{
			clearValues.resize(3);
			clearValues[0].color = { { 0.4f, 0.5f, 0.5f, 1.0f } };
			clearValues[1].color = { { 0.4f, 0.5f, 0.5f, 1.0f } };
			clearValues[2].depthStencil = { 1.0f, 0 };
		}

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		{
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = selectedPass;
			renderPassBeginInfo.framebuffer = selectedFramebuffer;
			renderPassBeginInfo.renderArea.extent.width = width;
			renderPassBeginInfo.renderArea.extent.height = height;
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();
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
		if(m_RenderpassFramebuffer)
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
		OpenglRendererAPI* instance = m_GraphicsContext->GetOpenglRendererAPI();
		instance->SetClearColor(clearColors);
		instance->Clear();
#else
		if (!m_RenderpassFramebuffer)
			return;

		VkClearRect clearRect = {};
		clearRect.layerCount = 1;
		clearRect.baseArrayLayer = 0;
		clearRect.rect.offset = { 0, 0 };
		clearRect.rect.extent = { (uint32_t)m_RenderpassFramebuffer->GetSpecification().Width, 
			(uint32_t)m_RenderpassFramebuffer->GetSpecification().Height };

		auto& framebuffer = m_RenderpassFramebuffer->GetVulkanFramebuffer();
		framebuffer.SetClearColors(clearColors);

		vkCmdClearAttachments(m_CommandBuffer, static_cast<uint32_t>(framebuffer.GetClearAttachments().size()),
			framebuffer.GetClearAttachments().data(), 1, &clearRect);
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

	void GraphicsPipeline::BeginCommandBuffer(bool isMainCmdBufferInUse)
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		if (isMainCmdBufferInUse)
		{
			m_CommandBuffer = VulkanContext::GetCurrentVkCmdBuffer();
			m_IsMainCmdBufferInUse = true;
			return;
		}

		m_CommandBuffer = VulkanCommandBuffer::CreateSingleCommandBuffer();
#else
		OpenglRendererAPI* instance = m_GraphicsContext->GetOpenglRendererAPI();
		instance->Init();
#endif
	}

	void GraphicsPipeline::EndCommandBuffer()
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		if (!m_IsMainCmdBufferInUse)
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

		OpenglRendererAPI* instance = m_GraphicsContext->GetOpenglRendererAPI();
		switch (mode)
		{
		case SmolEngine::DrawMode::Triangle:
			instance->DrawTriangle(m_VextexArray);
			break;
		case SmolEngine::DrawMode::Line:
			instance->DrawLine(m_VextexArray);
			break;
		case SmolEngine::DrawMode::Fan:
			instance->DrawFan(m_VextexArray);
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
		const auto& descriptorSets = m_VulkanPipeline.GetVkDescriptorSets(descriptorSetIndex);
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_VulkanPipeline.GetVkPipelineLayot(), 0, 1,
			&descriptorSets, 0, nullptr);

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

		OpenglRendererAPI* instance = m_GraphicsContext->GetOpenglRendererAPI();
		switch (mode)
		{
		case SmolEngine::DrawMode::Triangle:
			instance->DrawTriangle(m_VextexArray, 0, vertextCount);
			break;
		case SmolEngine::DrawMode::Line:
			instance->DrawLine(m_VextexArray, 0, vertextCount);
			break;
		case SmolEngine::DrawMode::Fan:
			instance->DrawFan(m_VextexArray, 0, vertextCount);
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
		const auto& descriptorSets = m_VulkanPipeline.GetVkDescriptorSets(descriptorSetIndex);
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_VulkanPipeline.GetVkPipelineLayot(), 0, 1,
			&descriptorSets, 0, nullptr);

		// Draw
		vkCmdDraw(m_CommandBuffer, vertextCount, 1, 0, 0);
#endif
	}

	void GraphicsPipeline::SumbitUniformBuffer(uint32_t bindingPoint, size_t size, const void* data, uint32_t offset)
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		m_Shader->SumbitUniformBuffer(bindingPoint, data, size, offset);
#endif

	}

	void GraphicsPipeline::SumbitPushConstant(ShaderType shaderStage, size_t size, const void* data)
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		vkCmdPushConstants(m_CommandBuffer, m_VulkanPipeline.GetVkPipelineLayot(), VulkanShader::GetVkShaderStage(shaderStage),
			0, static_cast<uint32_t>(size), data);
#endif
	}

	void GraphicsPipeline::SetVertexBuffers(std::vector<Ref<VertexBuffer>> buffer)
	{
		m_VertexBuffers = std::move(buffer);
	}

	void GraphicsPipeline::SetIndexBuffers(std::vector<Ref<IndexBuffer>> buffer)
	{
		m_IndexBuffers = std::move(buffer);
	}

	void GraphicsPipeline::UpdateVertextBuffer(void* vertices, size_t size, uint32_t bufferIndex, uint32_t offset)
	{
		m_VertexBuffers[bufferIndex]->UploadData(vertices, static_cast<uint32_t>(size), offset);
#ifdef SMOLENGINE_OPENGL_IMPL
		m_VextexArray->Bind();
		m_VextexArray->SetVertexBuffer(m_VertexBuffers[bufferIndex]);
#endif
	}

	void GraphicsPipeline::UpdateIndexBuffer(uint32_t* indices, size_t count, uint32_t bufferIndex, uint32_t offset)
	{
		m_IndexBuffers[bufferIndex]->UploadData(indices, static_cast<uint32_t>(count));
#ifdef SMOLENGINE_OPENGL_IMPL
		m_VextexArray->Bind();
		m_VextexArray->SetIndexBuffer(m_IndexBuffers[bufferIndex]);
#endif
	}

	bool GraphicsPipeline::UpdateSamplers(const std::vector<Ref<Texture>>& textures, uint32_t bindingPoint, uint32_t descriptorSetIndex)
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
		return m_VulkanPipeline.UpdateSamplers2D(vkTextures, bindingPoint, descriptorSetIndex);
#else
		return true;
#endif
	}

	bool GraphicsPipeline::UpdateSampler(Ref<Texture>& tetxure, uint32_t bindingPoint, uint32_t descriptorSetIndex)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		return false;
#else
		return m_VulkanPipeline.m_Descriptors[descriptorSetIndex].UpdateImageResource(bindingPoint,
			tetxure->GetVulkanTexture()->GetVkDescriptorImageInfo());
#endif
	}

	bool GraphicsPipeline::UpdateCubeMap(const Ref<Texture>& cubeMap, uint32_t bindingPoint, uint32_t descriptorSetIndex)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		return false; // temp
#else
		return m_VulkanPipeline.UpdateCubeMap(cubeMap->GetVulkanTexture(), bindingPoint, descriptorSetIndex);
#endif
	}

	// TODO: Add Debug Messages
	bool GraphicsPipeline::IsPipelineCreateInfoValid(const GraphicsPipelineCreateInfo* pipelineInfo)
	{
		return true;
	}
}