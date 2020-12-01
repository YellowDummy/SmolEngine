#pragma once
#include "Core/Core.h"

#include "Core/Renderer/Vulkan/VulkanInstance.h"
#include "Core/Renderer/Vulkan/VulkanDevice.h"
#include "Core/Renderer/Vulkan/VulkanCommandPool.h"
#include "Core/Renderer/Vulkan/VulkanSwapchain.h"
#include "Core/Renderer/Vulkan/VulkanCommandBuffer.h"
#include "Core/Renderer/Vulkan/VulkanSemaphore.h"

struct GLFWwindow;

namespace SmolEngine
{
	class VulkanContext
	{
	public:

		/// 
		/// Main
		/// 
		
		void OnResize(uint32_t width, uint32_t height);

		void Setup(GLFWwindow* window);

		void SwapBuffers();

		// TEMP
		
		void BuildTestCommandBuffer();

		void Render();

		/// 
		/// Getters
		/// 

		inline static VulkanCommandPool& GetCommandPool() { return m_CommandPool; }

		inline static VulkanContext* GetSingleton() { return s_ContextInstance; }

		inline static VulkanInstance& GetInstance() { return m_Instance; }

		inline static VulkanDevice& GetDevice() { return m_Device; }

		inline GLFWwindow* GetWindow() { return m_Window; }

	private:

		inline static VulkanContext* s_ContextInstance = nullptr;


		inline static VulkanCommandBuffer m_CommandBuffer = {};

		inline static VulkanCommandPool m_CommandPool = {};

		inline static VulkanSwapchain m_Swapchain = {};

		inline static VulkanSemaphore m_Semaphore = {};

		inline static VulkanInstance m_Instance = {};

		inline static VulkanDevice m_Device = {};


		GLFWwindow* m_Window = nullptr;

		uint32_t m_CurrentBuffer = 0;


		bool m_IsInitialized = false;
	};
}