#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Common/Core.h"
#include "Vulkan/Vulkan.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class VulkanDevice;
	class VulkanCommandBuffer;

	class VulkanSemaphore
	{
	public:

		VulkanSemaphore();
		~VulkanSemaphore();

		bool Init(const VulkanDevice* device, const VulkanCommandBuffer* commandBuffer);
		static void CreateVkSemaphore(VkSemaphore& outSemapthore);

		// Getters
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
#endif