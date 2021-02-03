#pragma once
#include "Core/Core.h"
#include "Renderer/Vulkan/Vulkan.h"

namespace SmolEngine
{
	class VulkanDevice;
	class VulkanCommandBuffer;

	class VulkanSemaphore
	{
	public:

		VulkanSemaphore();

		~VulkanSemaphore();

		/// Init
		
		bool Init(const VulkanDevice* device, const VulkanCommandBuffer* commandBuffer);

		/// Helpres

		static void CreateVkSemaphore(VkSemaphore& outSemapthore);

		/// Getters

		const VkSemaphore GetPresentCompleteSemaphore() const;

		const VkSemaphore GetRenderCompleteSemaphore() const;

		const std::vector<VkFence>& GetVkFences() const;

		const VkSubmitInfo* GetSubmitInfo() const;

	private:

		VkSubmitInfo             m_SubmitInfo = {};
		VkSemaphore              m_PresentComplete = nullptr;
		VkSemaphore              m_RenderComplete = nullptr;

		std::vector<VkFence>     m_WaitFences;
	};
}