#pragma once
#include "Core/Core.h"

#include "Renderer/FramebufferSpecification.h"
#include "Renderer/Vulkan/Vulkan.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	struct FrameBufferAttachment 
	{
		VkImage image;
		VkDeviceMemory mem;
		VkImageView view;
	};

	struct OffscreenPass
	{
		FrameBufferAttachment color, depth, resolve;

		VkSampler sampler;
		VkClearAttachment clearAttachments[3] = {};
	};

	class VulkanFramebuffer
	{
	public:

		VulkanFramebuffer();

		~VulkanFramebuffer();

		///  Main
		
		bool Init(const FramebufferSpecification& data);

		void OnResize(uint32_t width, uint32_t height);

	private:

		bool Create(uint32_t width, uint32_t height);

		void FreeResources();

	public:

		/// Setters

		void SetClearColors(const glm::vec4& clearColors);

		/// Getters

		const FramebufferSpecification& GetSpecification() const;

		const VkFramebuffer GetCurrentVkFramebuffer() const;

		const OffscreenPass& GetOffscreenPass() const;

		void* GetImGuiTextureID() const;

	private:

		FramebufferSpecification              m_Specification = {};
		OffscreenPass                         m_OffscreenPass = {};
		std::vector<VkFramebuffer>            m_VkFrameBuffers;
		void*                                 m_ImGuiTextureID = nullptr;
		VkDevice                              m_Device = nullptr;
		VkSampleCountFlagBits                 m_MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	private:

		friend class GraphicsPipeline;
		friend class VulkanSwapchain;
	};
}