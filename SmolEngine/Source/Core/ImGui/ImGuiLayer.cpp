#include "stdafx.h"
#include "ImGuiLayer.h"

#define GLFW_INCLUDE_VULKAN
#define IMGUI_VULKAN_DEBUG_REPORT

#include <../Libraries/imgui/imgui.h>
#include "../Libraries/imgui/imgui_internal.h"
#include <../Libraries/imgui/examples/imgui_impl_glfw.h>
#include <../Libraries/imgui/examples/imgui_impl_opengl3.h>
#include <../Libraries/imgui/examples/imgui_impl_vulkan.h>

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "Core/Window.h"
#include "Core/EventHandler.h"
#include "Core/Application.h"
#include "IconsFontAwesome5.h"
#include "Core/Window.h"

#include "NodeEditor/imnodes.h"

namespace SmolEngine 
{

	ImGuiLayer::ImGuiLayer()
		:Layer("ImGuiLayer")
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		if (io.WantTextInput && event.m_EventCategory == (uint32_t)EventCategory::S_EVENT_KEYBOARD)
		{
			event.m_Handled = true;
		}

		//TO DO: Block Mouse & Keyboard events
		//event.m_Handled |= event.m_EventCategory == (uint32_t) EventType::S_MOUSE_MOVE && io.WantCaptureMouse;
		//event.m_Handled |= event.m_EventCategory == (uint32_t)EventCategory::S_EVENT_KEYBOARD && io.WantCaptureKeyboard;
	}

	void ImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		imnodes::Initialize();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.ChildRounding = 3.f;
			style.GrabRounding = 0.f;
			style.WindowRounding = 0.f;
			style.ScrollbarRounding = 3.f;
			style.FrameRounding = 3.f;
			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);


			style.Colors[ImGuiCol_Text] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			//style.Colors[ImGuiCol_ComboBg] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);

			//style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
			//style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			//style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
			style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);

			style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		}

		ImFontConfig config;
		config.OversampleH = 3;
		config.OversampleV = 1;
		config.GlyphExtraSpacing.x = 1.0f;

#ifdef SMOLENGINE_EDITOR

		io.Fonts->AddFontFromFileTTF("../SmolEngine/Assets/Fonts/Font1.ttf", 17.0f, &config);
#else
		io.Fonts->AddFontFromFileTTF("../../SmolEngine/Assets/Fonts/Font1.ttf", 17.0f, &config);
#endif

		Application& app = Application::GetApplication();
		GLFWwindow* EditorWindow = app.GetWindow().GetNativeWindow();

#ifdef  SMOLENGINE_OPENGL_IMPL


		ImGui_ImplGlfw_InitForOpenGL(EditorWindow, true);

		ImGui_ImplOpenGL3_Init("#version 410");
#else
		ImGui_ImplGlfw_InitForVulkan(EditorWindow, true);

		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		{
			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.pNext = nullptr;
			descriptorPoolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
			descriptorPoolInfo.pPoolSizes = pool_sizes;
			descriptorPoolInfo.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);

			VK_CHECK_RESULT(vkCreateDescriptorPool(*VulkanContext::GetDevice().GetLogicalDevice(), &descriptorPoolInfo, nullptr, &m_DescriptorPool));
		}

		VkPipelineCacheCreateInfo pipelineCacheCI = {};
		{
			pipelineCacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

			VK_CHECK_RESULT(vkCreatePipelineCache(*VulkanContext::GetDevice().GetLogicalDevice(), &pipelineCacheCI, nullptr, &m_PipelineCache));
		}

		VkAttachmentDescription attachment = {};
		{
			attachment.format = VulkanContext::GetSwapchain().GetColorFormat();
			attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}

		VkAttachmentReference color_attachment = {};
		{
			color_attachment.attachment = 0;
			color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		VkSubpassDescription subpass = {};
		{
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &color_attachment;
		}

		VkSubpassDependency dependency = {};
		{
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}

		VkRenderPassCreateInfo info = {};
		{
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			info.attachmentCount = 1;
			info.pAttachments = &attachment;
			info.subpassCount = 1;
			info.pSubpasses = &subpass;
			info.dependencyCount = 1;
			info.pDependencies = &dependency;

			VK_CHECK_RESULT(vkCreateRenderPass(*VulkanContext::GetDevice().GetLogicalDevice(), &info, nullptr, &m_RenderPass));
		}

		ImGui_ImplVulkan_InitInfo init_info = {};
		{
			init_info.Instance = *VulkanContext::GetInstance().GetInstance();
			init_info.PhysicalDevice = *VulkanContext::GetDevice().GetPhysicalDevice();
			init_info.Device = *VulkanContext::GetDevice().GetLogicalDevice();
			init_info.QueueFamily = VulkanContext::GetDevice().GetQueueFamilyIndex();
			init_info.Queue = *VulkanContext::GetDevice().GetQueue();
			init_info.DescriptorPool = m_DescriptorPool;
			init_info.PipelineCache = m_PipelineCache;
			init_info.Allocator = nullptr;
			init_info.MinImageCount = 2; // temp
			init_info.ImageCount = 3; // temp
		}

		ImGui_ImplVulkan_Init(&init_info, m_RenderPass);

		ImGui_ImplVulkan_CreateFontsTexture(VulkanContext::GetCommandBuffer().GetVkCommandBuffer());

#endif //  SMOLENGINE_OPENGL_IMPL
	}

	void ImGuiLayer::OnDetach()
	{
#ifdef  SMOLENGINE_OPENGL_IMPL

		ImGui_ImplOpenGL3_Shutdown();
#else

		ImGui_ImplVulkan_Shutdown();

#endif

		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		imnodes::Shutdown();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::OnBegin()
	{
#ifdef  SMOLENGINE_OPENGL_IMPL

		ImGui_ImplOpenGL3_NewFrame();
#else
		ImGui_ImplVulkan_NewFrame();
#endif
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}


	void ImGuiLayer::OnEnd()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)Data.Width, (float)Data.Height);

		ImGui::Render();

#ifdef  SMOLENGINE_OPENGL_IMPL

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), VulkanContext::GetCommandBuffer().GetVkCommandBuffer());
#endif

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

#ifndef SMOLENIGNE_OPENGL_IMPL

#if 0
	void ImGuiLayer::FrameRender(VulkanSwapchain* wd, ImDrawData* draw_data, VulkanSemaphore* semaphore)
	{
		VkResult err;

		VkSemaphore image_acquired_semaphore = semaphore->GetPresentCompleteSemaphore();
		VkSemaphore render_complete_semaphore = semaphore->GetRenderCompleteSemaphore();
		err = vkAcquireNextImageKHR(*VulkanContext::GetDevice().GetLogicalDevice(), wd->GetVkSwapchain(), UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->GetCurrentBufferIndexRef());
		if (err == VK_ERROR_OUT_OF_DATE_KHR)
		{
			m_SwapChainRebuild = true;
			return;
		}

		ImGui_ImplVulkanH_Window;

		VK_CHECK_RESULT(err);

		ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
		{
			err = vkWaitForFences(*VulkanContext::GetDevice().GetLogicalDevice(), 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
			VK_CHECK_RESULT(err);

			err = vkResetFences(*VulkanContext::GetDevice().GetLogicalDevice(), 1, &fd->Fence);
			VK_CHECK_RESULT(err);
		}
		{
			err = vkResetCommandPool(*VulkanContext::GetDevice().GetLogicalDevice(), fd->CommandPool, 0);
			VK_CHECK_RESULT(err);
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
			VK_CHECK_RESULT(err);
		}
		{
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = wd->RenderPass;
			info.framebuffer = fd->Framebuffer;
			info.renderArea.extent.width = wd->Width;
			info.renderArea.extent.height = wd->Height;
			info.clearValueCount = 1;
			info.pClearValues = &wd->ClearValue;
			vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

		// Submit command buffer
		vkCmdEndRenderPass(fd->CommandBuffer);
		{
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			info.waitSemaphoreCount = 1;
			info.pWaitSemaphores = &image_acquired_semaphore;
			info.pWaitDstStageMask = &wait_stage;
			info.commandBufferCount = 1;
			info.pCommandBuffers = &fd->CommandBuffer;
			info.signalSemaphoreCount = 1;
			info.pSignalSemaphores = &render_complete_semaphore;

			err = vkEndCommandBuffer(fd->CommandBuffer);
			VK_CHECK_RESULT(err);
			err = vkQueueSubmit(*VulkanContext::GetDevice().GetQueue(), 1, &info, fd->Fence);
			VK_CHECK_RESULT(err);
		}
	}

	void ImGuiLayer::FramePresent(VulkanSwapchain* wd, VulkanSemaphore* semaphore)
	{
		if (m_SwapChainRebuild)
			return;
		VkSemaphore render_complete_semaphore = wd->Ger;
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &render_complete_semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &wd->Swapchain;
		info.pImageIndices = &wd->FrameIndex;
		VkResult err = vkQueuePresentKHR(*VulkanContext::GetDevice().GetQueue(), &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR)
		{
			m_SwapChainRebuild = true;
			return;
		}

		VK_CHECK_RESULT(err);
		wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
	}

#endif

#endif

}
