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

		inline static VkRenderPass GetMSAAVkRenderPassFramebufferLayout() { return m_MSAARenderPassFramebuffer; }

		inline static VkRenderPass GetMSAAVkRenderPassSwapchainLayout() { return m_MSAARenderPassSwapchain; }

		inline static VkRenderPass GetVkRenderPassFramebufferLayout() { return m_RenderPassFramebuffer; }

		inline static VkRenderPass GetVkRenderPassSwapchainLayout() { return m_RenderPassSwapchain; }

		inline static VkRenderPass GetVkRenderPassDeferredLayout() { return m_DeferredRenderPass; }

		static VkRenderPass& GetRenderPass(bool targetsSwapchain, bool useMSAA, bool useMRT);

	private:

		struct RenderPassCI
		{
			VkImageLayout                 ColorAttachmentFinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			VkImageLayout                 ResolveAttachmentFinalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			VkImageLayout                 DepthAttachmentFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			bool                          IsUseMRT = false;
			bool                          IsUseMSAA = true;
		};

		static void CreateRenderPass(RenderPassCI& renderPassCI, VkRenderPass& renderPass);

	private:

		inline static VkRenderPass        m_MSAARenderPassFramebuffer = nullptr;
		inline static VkRenderPass        m_MSAARenderPassSwapchain = nullptr;
		inline static VkRenderPass        m_RenderPassFramebuffer = nullptr;
		inline static VkRenderPass        m_RenderPassSwapchain = nullptr;
		inline static VkRenderPass        m_DeferredRenderPass = nullptr;
	};
}