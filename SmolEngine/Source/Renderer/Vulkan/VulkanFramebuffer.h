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
		FrameBufferAttachment color, depth;
		std::array<VkClearAttachment, 2> clearAttachments;
	};

	struct DeferredPass
	{
		FrameBufferAttachment position, normals, color, depth;
		VkDescriptorImageInfo positionImageInfo, normalsImageInfo, colorImageInfo;
		VkSemaphore semaphore;

		std::array<VkClearAttachment, 4> clearAttachments;
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

		bool CreateDeferred();

		void CreateSampler();

		void FreeResources();

		void AddAttachment(uint32_t width, uint32_t height, VkSampleCountFlagBits samples,
			VkImageUsageFlags imageUsage,
			VkFormat format, VkImage& image, VkImageView& imageView, VkDeviceMemory& mem, VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT);

		void FreeAttachment(FrameBufferAttachment& framebuffer);

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
		DeferredPass                          m_DeferredPass = {};
		std::vector<VkFramebuffer>            m_VkFrameBuffers;
		void*                                 m_ImGuiTextureID = nullptr;
		VkDevice                              m_Device = nullptr;
		VkSampler                             m_Sampler = nullptr;
		VkSampleCountFlagBits                 m_MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		VkFormat                              m_ColorFormat;
		VkFormat                              m_DepthFormat;

	private:

		friend class GraphicsPipeline;
		friend class VulkanSwapchain;
	};
}