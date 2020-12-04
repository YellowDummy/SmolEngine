#pragma once

#include "Core/Layer.h"
#include "Core/Core.h"

#ifndef SMOLENIGNE_OPENGL_IMPL
#include "Core/Renderer/Vulkan/VulkanContext.h"

#endif

namespace SmolEngine 
{
	class Window;

	class ImGuiLayer: public Layer
	{
	public:

		ImGuiLayer();

		~ImGuiLayer();

		void OnEvent(Event& event) override;

		void OnAttach() override;

		void OnDetach() override;

		void OnImGuiRender() override;

		void OnBegin();

		void OnEnd();

#ifndef SMOLENIGNE_OPENGL_IMPL

#if 0
		void FrameRender(VulkanSwapchain* wd, ImDrawData* draw_data, VulkanSemaphore* semaphore);

		void FramePresent(VulkanSwapchain* wd, VulkanSemaphore* semaphore);

#endif

#endif

	private:

#ifndef SMOLENIGNE_OPENGL_IMPL

		VkRenderPass m_RenderPass = nullptr;

		VkDescriptorPool m_DescriptorPool = nullptr;

		VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;


		bool m_SwapChainRebuild = false;
#endif
	};

}
