#pragma once
#include "Core/Core.h"

#include "Renderer/FramebufferSpecification.h"
#include "Renderer/Vulkan/Vulkan.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	struct VulkanFramebufferAttachment
	{
		VkImage                image;
		VkDeviceMemory         mem;
		VkImageView            view;
	};

	struct Attachment
	{
		VulkanFramebufferAttachment   AttachmentVkInfo;
		VkDescriptorImageInfo         ImageInfo;

		void*                         ImGuiID = nullptr;
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

		void CreateSampler();

		void FreeResources();

		void AddAttachment(uint32_t width, uint32_t height, VkSampleCountFlagBits samples,
			VkImageUsageFlags imageUsage,
			VkFormat format, VkImage& image, VkImageView& imageView, VkDeviceMemory& mem, VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT);

		void FreeAttachment(Attachment& framebuffer);

	public:

		/// Setters

		void SetClearColors(const glm::vec4& clearColors);

		/// Getters

		const std::vector<VkClearAttachment>& GetClearAttachments() const;

		const std::vector<VkClearValue>& GetClearValues() const;

		const FramebufferSpecification& GetSpecification() const;

		const VkFramebuffer GetCurrentVkFramebuffer() const;

		static VkFormat GetAttachmentFormat(AttachmentFormat format);

		VkRenderPass GetRenderPass() const;

		VkSampleCountFlagBits GetMSAASamples() const;

		Attachment* GetAttachment(uint32_t index = 0);

		Attachment* GetAttachment(std::string& name);

	private:

		Attachment                                       m_ResolveAttachment = {};
		Attachment                                       m_DepthAttachment = {};
											             
		FramebufferSpecification                         m_Specification = {};
		VkDevice                                         m_Device = nullptr;
		VkSampler                                        m_Sampler = nullptr;
		VkSampleCountFlagBits                            m_MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		VkFormat                                         m_ColorFormat;
		VkFormat                                         m_DepthFormat;
		VkRenderPass                                     m_RenderPass = nullptr;
											             
		std::vector<VkFramebuffer>                       m_VkFrameBuffers;
		std::vector<VkClearAttachment>                   m_ClearAttachments;
		std::vector<VkClearValue>                        m_ClearValues;
		std::vector<Attachment>                          m_ColorAttachments;
		std::unordered_map<std::string, uint32_t>        m_ColorAttachmentsMap;

	private:

		friend class GraphicsPipeline;
		friend class VulkanSwapchain;
	};
}