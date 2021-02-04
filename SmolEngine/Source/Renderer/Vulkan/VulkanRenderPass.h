#pragma once
#include "Core/Core.h"
#include "Renderer/Vulkan/Vulkan.h"

namespace SmolEngine
{
	class VulkanRenderPass
	{
	public:

		static void Init();

		static void Free();

		// Getters

		inline static VkRenderPass GetVkRenderPassFramebufferLayout() { return m_RenderPassFramebuffer; }

		inline static VkRenderPass GetVkRenderPassSwapchainLayout() { return m_RenderPassSwapchain; }

		inline static VkRenderPass GetVkRenderPassDeferredLayout() { return m_MSAADeferredRenderPass; }

	private:

		struct RenderPassCI
		{
			VkImageLayout                 ColorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			VkImageLayout                 ResolveAttachmentFinalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			VkImageLayout                 DepthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			bool                          IsUseMRT = false;
		};

		static void CreateRenderPass(RenderPassCI& renderPassCI, VkRenderPass& renderPass);

	private:

		inline static VkRenderPass        m_RenderPassFramebuffer = nullptr;
		inline static VkRenderPass        m_RenderPassSwapchain = nullptr;
		inline static VkRenderPass        m_MSAADeferredRenderPass = nullptr;
	};
}