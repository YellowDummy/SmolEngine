#include "stdafx.h"
#include "VulkanSwapchain.h"
#include "Core/Renderer/Vulkan/VulkanInstance.h"
#include "Core/Renderer/Vulkan/VulkanDevice.h"

#include <GLFW/glfw3.h>

static PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR = nullptr;
static PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
static PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
static PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
static PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR = nullptr;
static PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR = nullptr;
static PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR = nullptr;
static PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR = nullptr;
static PFN_vkQueuePresentKHR fpQueuePresentKHR = nullptr;

namespace SmolEngine
{
    VulkanSwapchain::VulkanSwapchain()
    {

    }
    VulkanSwapchain::~VulkanSwapchain()
    {

    }

    bool VulkanSwapchain::Init(VulkanInstance* instance, VulkanDevice* device, GLFWwindow* window)
    {
        m_Instance = instance;
		m_Device = device;

        fpGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(vkGetInstanceProcAddr(*m_Instance->GetInstance(), "vkGetPhysicalDeviceSurfaceSupportKHR"));
        fpGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(vkGetInstanceProcAddr(*m_Instance->GetInstance(), "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
        fpGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vkGetInstanceProcAddr(*m_Instance->GetInstance(), "vkGetPhysicalDeviceSurfaceFormatsKHR"));
        fpGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(vkGetInstanceProcAddr(*m_Instance->GetInstance(), "vkGetPhysicalDeviceSurfacePresentModesKHR"));

        fpCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(vkGetDeviceProcAddr(*m_Device->GetLogicalDevice(), "vkCreateSwapchainKHR"));
        fpDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(vkGetDeviceProcAddr(*m_Device->GetLogicalDevice(), "vkDestroySwapchainKHR"));
        fpGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vkGetDeviceProcAddr(*m_Device->GetLogicalDevice(), "vkGetSwapchainImagesKHR"));
        fpAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(vkGetDeviceProcAddr(*m_Device->GetLogicalDevice(), "vkAcquireNextImageKHR"));
        fpQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(vkGetDeviceProcAddr(*m_Device->GetLogicalDevice(), "vkQueuePresentKHR"));

		if (InitSurface(window) == VK_SUCCESS)
		{
			return true;
		}

		return false;
    }

	void VulkanSwapchain::Create(uint32_t* width, uint32_t* height, bool vSync)
	{
		if (m_Device == nullptr || m_Instance == nullptr)
		{
			NATIVE_ERROR("VulkanDevice or VulkanInstance is nullptr");
			assert(m_Device != nullptr || m_Instance != nullptr);
			return;
		}

		VkSwapchainKHR oldSwapchain = m_Swapchain;

		// Get physical device surface properties and formats
		VkSurfaceCapabilitiesKHR surfCaps;
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfaceCapabilitiesKHR(*m_Device->GetPhysicalDevice(), m_Surface, &surfCaps));

		// Get available present modes
		uint32_t presentModeCount;
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfacePresentModesKHR(*m_Device->GetPhysicalDevice(), m_Surface, &presentModeCount, NULL));
		assert(presentModeCount > 0);

		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfacePresentModesKHR(*m_Device->GetPhysicalDevice(), m_Surface, &presentModeCount, presentModes.data()));

		VkExtent2D swapchainExtent = {};
		// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
		if (surfCaps.currentExtent.width == (uint32_t)-1)
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = *width;
			swapchainExtent.height = *height;
		}
		else
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfCaps.currentExtent;
			*width = surfCaps.currentExtent.width;
			*height = surfCaps.currentExtent.height;
		}


		// Select a present mode for the swapchain

		// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
		// This mode waits for the vertical blank ("v-sync")
		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

		// If v-sync is not requested, try to find a mailbox mode
		// It's the lowest latency non-tearing present mode available
		if (!vSync)
		{
			for (size_t i = 0; i < presentModeCount; i++)
			{
				if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				}
				if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
				{
					swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
			}
		}

		// Determine the number of images
		uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
		if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
		{
			desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
		}

		// Find the transformation of the surface
		VkSurfaceTransformFlagsKHR preTransform;
		if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			// We prefer a non-rotated transform
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else
		{
			preTransform = surfCaps.currentTransform;
		}

		// Find a supported composite alpha format (not all devices support alpha opaque)
		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// Simply select the first composite alpha format available
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (auto& compositeAlphaFlag : compositeAlphaFlags) {
			if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) {
				compositeAlpha = compositeAlphaFlag;
				break;
			};
		}

		VkSwapchainCreateInfoKHR swapchainCI = {};
		{
			swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchainCI.pNext = NULL;
			swapchainCI.surface = m_Surface;
			swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
			swapchainCI.imageFormat = m_ColorFormat;
			swapchainCI.imageColorSpace = m_ColorSpace;
			swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };
			swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
			swapchainCI.imageArrayLayers = 1;
			swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCI.queueFamilyIndexCount = 0;
			swapchainCI.pQueueFamilyIndices = NULL;
			swapchainCI.presentMode = swapchainPresentMode;
			swapchainCI.oldSwapchain = oldSwapchain;

			// Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
			swapchainCI.clipped = VK_TRUE;
			swapchainCI.compositeAlpha = compositeAlpha;

			// Enable transfer source on swap chain images if supported
			if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
				swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			}

			// Enable transfer destination on swap chain images if supported
			if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
				swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			}

			VK_CHECK_RESULT(fpCreateSwapchainKHR(*m_Device->GetLogicalDevice(), &swapchainCI, nullptr, &m_Swapchain));
		}

		// If an existing swap chain is re-created, destroy the old swap chain
		// This also cleans up all the presentable images
		if (oldSwapchain != VK_NULL_HANDLE)
		{
			for (uint32_t i = 0; i < m_ImageCount; i++)
			{
				vkDestroyImageView(*m_Device->GetLogicalDevice(), m_Buffers[i].View, nullptr);
			}

			fpDestroySwapchainKHR(*m_Device->GetLogicalDevice(), oldSwapchain, nullptr);
		}

		VK_CHECK_RESULT(fpGetSwapchainImagesKHR(*m_Device->GetLogicalDevice(), m_Swapchain, &m_ImageCount, NULL));

		// Get the swap chain images
		m_Images.resize(m_ImageCount);
		VK_CHECK_RESULT(fpGetSwapchainImagesKHR(*m_Device->GetLogicalDevice(), m_Swapchain, &m_ImageCount, m_Images.data()));

		// Get the swap chain buffers containing the image and imageview
		m_Buffers.resize(m_ImageCount);
		for (uint32_t i = 0; i < m_ImageCount; i++)
		{
			VkImageViewCreateInfo colorAttachmentView = {};
			{
				colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				colorAttachmentView.pNext = NULL;
				colorAttachmentView.format = m_ColorFormat;
				colorAttachmentView.components = {
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_G,
					VK_COMPONENT_SWIZZLE_B,
					VK_COMPONENT_SWIZZLE_A
				};
				colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				colorAttachmentView.subresourceRange.baseMipLevel = 0;
				colorAttachmentView.subresourceRange.levelCount = 1;
				colorAttachmentView.subresourceRange.baseArrayLayer = 0;
				colorAttachmentView.subresourceRange.layerCount = 1;
				colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
				colorAttachmentView.flags = 0;

				m_Buffers[i].Image = m_Images[i];

				colorAttachmentView.image = m_Buffers[i].Image;
			}

			VK_CHECK_RESULT(vkCreateImageView(*m_Device->GetLogicalDevice(), &colorAttachmentView, nullptr, &m_Buffers[i].View));
		}
	}

	void VulkanSwapchain::CleanUp()
	{
		if (m_Device == nullptr || m_Instance == nullptr)
		{
			NATIVE_ERROR("VulkanDevice or VulkanInstance is nullptr");
			assert(m_Device != nullptr || m_Instance != nullptr);
			return;
		}

		if (m_Swapchain != VK_NULL_HANDLE)
		{
			for (uint32_t i = 0; i < m_ImageCount; i++)
			{
				vkDestroyImageView(*m_Device->GetLogicalDevice(), m_Buffers[i].View, nullptr);
			}
		}

		if (m_Surface != VK_NULL_HANDLE)
		{
			fpDestroySwapchainKHR(*m_Device->GetLogicalDevice(), m_Swapchain, nullptr);
			vkDestroySurfaceKHR(*m_Instance->GetInstance(), m_Surface, nullptr);
		}

		m_Surface = VK_NULL_HANDLE;
		m_Swapchain = VK_NULL_HANDLE;
	}

	VkResult VulkanSwapchain::AcquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex)
	{
		if (m_Device == nullptr || m_Instance == nullptr)
		{
			NATIVE_ERROR("VulkanDevice or VulkanInstance is nullptr");
			assert(m_Device != nullptr || m_Instance != nullptr);
			return VK_ERROR_UNKNOWN;
		}

		// By setting timeout to UINT64_MAX we will always wait until the next image has been acquired or an actual error is thrown
		// With that we don't have to handle VK_NOT_READY
		return fpAcquireNextImageKHR(*m_Device->GetLogicalDevice(), m_Swapchain, UINT64_MAX, presentCompleteSemaphore, (VkFence)nullptr, imageIndex);
	}

	VkResult VulkanSwapchain::QueuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore)
	{
		if (m_Device == nullptr || m_Instance == nullptr)
		{
			NATIVE_ERROR("VulkanDevice or VulkanInstance is nullptr");
			assert(m_Device != nullptr || m_Instance != nullptr);
			return VK_ERROR_UNKNOWN;
		}

		VkPresentInfoKHR presentInfo = {};
		{
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pNext = NULL;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &m_Swapchain;
			presentInfo.pImageIndices = &imageIndex;

			// Check if a wait semaphore has been specified to wait for before presenting the image
			if (waitSemaphore != VK_NULL_HANDLE)
			{
				presentInfo.pWaitSemaphores = &waitSemaphore;
				presentInfo.waitSemaphoreCount = 1;
			}

			return fpQueuePresentKHR(queue, &presentInfo);
		}
	}

    VkResult VulkanSwapchain::InitSurface(GLFWwindow* window)
    {
		VkResult result = glfwCreateWindowSurface(*m_Instance->GetInstance(), window, nullptr, &m_Surface);
		VK_CHECK_RESULT(result);

#if 0
        // Create surface
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
        {
            surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            surfaceCreateInfo.hinstance = (HINSTANCE)platformHandle;
            surfaceCreateInfo.hwnd = (HWND)platformWindow;
            VkResult result = vkCreateWin32SurfaceKHR(*m_Instance->GetInstance(), &surfaceCreateInfo, nullptr, &m_Surface);

            assert(result == VK_SUCCESS);
        }

		// Get available queue family properties
		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(*m_Device->GetPhysicalDevice(), &queueCount, NULL);
		assert(queueCount >= 1);

		std::vector<VkQueueFamilyProperties> queueProps(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(*m_Device->GetPhysicalDevice(), &queueCount, queueProps.data());

		// Iterate over each queue to learn whether it supports presenting:
		// Find a queue with present support
		// Will be used to present the swap chain images to the windowing system
		std::vector<VkBool32> supportsPresent(queueCount);
		for (uint32_t i = 0; i < queueCount; i++)
		{
			fpGetPhysicalDeviceSurfaceSupportKHR(*m_Device->GetPhysicalDevice(), i, m_Surface, &supportsPresent[i]);
		}

		// Search for a graphics and a present queue in the array of queue
		// families, try to find one that supports both
		uint32_t graphicsQueueNodeIndex = UINT32_MAX;
		uint32_t presentQueueNodeIndex = UINT32_MAX;
		for (uint32_t i = 0; i < queueCount; i++)
		{
			if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				if (graphicsQueueNodeIndex == UINT32_MAX)
				{
					graphicsQueueNodeIndex = i;
				}

				if (supportsPresent[i] == VK_TRUE)
				{
					graphicsQueueNodeIndex = i;
					presentQueueNodeIndex = i;
					break;
				}
			}
		}
		if (presentQueueNodeIndex == UINT32_MAX)
		{
			// If there's no queue that supports both present and graphics
			// try to find a separate present queue
			for (uint32_t i = 0; i < queueCount; ++i)
			{
				if (supportsPresent[i] == VK_TRUE)
				{
					presentQueueNodeIndex = i;
					break;
				}
			}
		}

		// Exit if either a graphics or a presenting queue hasn't been found
		if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX)
		{
			NATIVE_ERROR("Could not find a graphics and/or presenting queue!");
			abort();
		}

		// todo : Add support for separate graphics and presenting queue
		if (graphicsQueueNodeIndex != presentQueueNodeIndex)
		{
			NATIVE_ERROR("Separate graphics and presenting queues are not supported yet!");
			abort();
		}

		m_QueueNodeIndex = graphicsQueueNodeIndex;

#endif
		FindColorSpaceFormat();

		return result;
    }

	void VulkanSwapchain::FindColorSpaceFormat()
	{
		// Get list of supported surface formats
		uint32_t formatCount;
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfaceFormatsKHR(*m_Device->GetPhysicalDevice(), m_Surface, &formatCount, NULL));
		assert(formatCount > 0);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfaceFormatsKHR(*m_Device->GetPhysicalDevice(), m_Surface, &formatCount, surfaceFormats.data()));

		// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
		// there is no preferred format, so we assume VK_FORMAT_B8G8R8A8_UNORM
		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			m_ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
			m_ColorSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			// iterate over the list of available surface format and
			// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
			bool found_B8G8R8A8_UNORM = false;
			for (auto&& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
				{
					m_ColorFormat = surfaceFormat.format;
					m_ColorSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
					break;
				}
			}

			// in case VK_FORMAT_B8G8R8A8_UNORM is not available
			// select the first available color format
			if (!found_B8G8R8A8_UNORM)
			{
				m_ColorFormat = surfaceFormats[0].format;
				m_ColorSpace = surfaceFormats[0].colorSpace;
			}
		}
	}

}