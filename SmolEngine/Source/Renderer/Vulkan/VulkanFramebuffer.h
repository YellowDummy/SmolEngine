#pragma once
#include "Core/Core.h"

#include "Renderer/FramebufferSpecification.h"
#include "Renderer/Vulkan/Vulkan.h"

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
		VkFramebuffer frameBuffer;
		FrameBufferAttachment color, depth;

		VkSampler sampler;
		VkDescriptorImageInfo descriptor;
		VkClearAttachment clearAttachments[2] = {};
		VkRenderPass renderPass;
		void* ImGuiTextureID = nullptr;
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

		const OffscreenPass& GetOffscreenPass() const;

		void* GetImGuiTextureID() const;

	private:

		FramebufferSpecification m_Specification = {};
		OffscreenPass m_OffscreenPass = {};

		friend class GraphicsPipeline;
	};
}