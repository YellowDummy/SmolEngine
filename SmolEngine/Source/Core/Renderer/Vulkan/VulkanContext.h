#pragma once
#include "Core/Core.h"

#include "Core/Renderer/Vulkan/VulkanInstance.h"
#include "Core/Renderer/Vulkan/VulkanDevice.h"
#include "Core/Renderer/Vulkan/VulkanCommandPool.h"
#include "Core/Renderer/Vulkan/VulkanSwapchain.h"
#include "Core/Renderer/Vulkan/VulkanCommandBuffer.h"

struct GLFWwindow;

namespace SmolEngine
{
	class VulkanContext
	{
	public:

		/// 
		/// Main
		/// 
		
		void Setup(GLFWwindow* window);

		void SwapBuffers();

		/// 
		/// Getters
		/// 

		inline static VulkanCommandPool& GetCommandPool() { return m_CommandPool; }

		inline static VulkanInstance& GetInstance() { return m_Instance; }

		inline static VulkanDevice& GetDevice() { return m_Device; }

		inline GLFWwindow* GetWindow() { return m_Window; }

	private:

		inline static VulkanCommandPool m_CommandPool = {};

		inline static VulkanInstance m_Instance = {};

		inline static VulkanDevice m_Device = {};

		inline static VulkanSwapchain m_Swapchain = {};

		inline static VulkanCommandBuffer m_CommandBuffer = {};


		GLFWwindow* m_Window = nullptr;
	};
}